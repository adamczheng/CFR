#include "states.hpp"
#include "actions.hpp"
#include "Fold.cpp"
#include "InfoSet.h"
#include <random>
#include <chrono>
#include <iostream>
#include <cassert>
#include <array>
class Decision : public State {
private:
	int player;
	vector<State*> children;
	InfoSet& iset;
	mt19937 rng;
public:
	Decision(int pl, vector<State*> c, InfoSet is) {
		player = pl;
		children = c;
		iset = is;
		rng.seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());
		assert(c.size() > 0);
	}

	int SampleStrategy(const vector<double>& s) {
		uniform_real_distribution<double> distribution(0, 1);
		double r = distribution(rng);
		double acc = 0;
		for (int i = 0; i < (int)s.size(); i++) {
			acc += s[i];
			if (r < acc) return i;
		}
		assert(0);
	}

	virtual double TrainExternalSampling(int trainplayer, int* hands, double p, double op) override {
		//std::cout << "Decision" << std::endl;
		if (player == trainplayer) {
			vector<double> s = GetStrategy(hands[player]);
			int n_children = children.size();
			for (int i = 0; i < n_children; i++)
				iset.cumulativeStrategy[hands[player]][i] += p * s[i] / op;
			vector<double> u(n_children);
			double ev = 0;
			for (int i = 0; i < n_children; i++) {
				u[i] = children[i]->TrainExternalSampling(trainplayer, hands, p * s[i], op);
				ev += u[i] * s[i];
			}
			for (int i = 0; i < n_children; i++)
				regret[hands[player]][i] += u[i] - ev;
			return ev;
		}
		else {
			vector<double> s = GetStrategy(hands[player]);
			//double ev = 0;
			//int n_children = children.size();
			//for (int i = 0; i < n_children; i++) {
			//	ev += children[i]->TrainExternalSampling(trainplayer, hands, p, op * s[i]);
			//}
			//return ev;
			int a = SampleStrategy(s);
			//std::cout << s[a] << std::endl;
			return children[a]->TrainExternalSampling(trainplayer, hands, p, op * s[a]);
		}
	}

	vector<double> GetStrategy(int hand) {
		int n_children = children.size();

		vector<double> s(n_children);
		double psum = 0;
		for (int i = 0; i < n_children; i++)
			if (regret[hand][i] > 0)
				psum += regret[hand][i];

		if (psum > 0) {
			for (int i = 0; i < n_children; i++)
				s[i] = (regret[hand][i] > 0) ? regret[hand][i] / psum : 0.0;
		}
		else {
			for (int i = 0; i < n_children; i++)
				s[i] = 1.0 / n_children;
		}

		return s;
	}
	vector<double> GetNormalizedAverageStrategy(int hand) {
		const double THRESHOLD = 0.01; // make actions with p < THRESHOLD never happen
		int n_children = children.size();
		vector<double> s(n_children);
		bool flag = false;
		double sum = 0;
		for (int i = 0; i < n_children; i++)
			sum += cumulativeStrategy[hand][i] > 0 ? cumulativeStrategy[hand][i] : 0.0;
		if (sum > 0)
			for (int i = 0; i < n_children; i++) {
				if (cumulativeStrategy[hand][i] > 0 && cumulativeStrategy[hand][i] / sum >= THRESHOLD) {
					flag = true;
					s[i] = cumulativeStrategy[hand][i] / sum;
				}
				else {
					s[i] = 0;
				}
			}
		else
			for (int i = 0; i < n_children; i++)
				s[i] = 1.0 / n_children;
		if (flag) {
			sum = 0;
			for (int i = 0; i < n_children; i++)
				sum += s[i];;
			if (sum > 0)
				for (int i = 0; i < n_children; i++) {
					if (s[i] > 0) {
						s[i] = s[i] / sum;
					}
					else {
						s[i] = 0;
					}
				}
			else
				for (int i = 0; i < n_children; i++)
					s[i] = 1.0 / n_children;
		}
		return s;
	}
	double BestResponse(int brplayer) {
		double sum = 0;
		for (int hand = 0; hand < DECK_SIZE; hand++) {
			vector<double> op(DECK_SIZE);
			for (int i = 0; i < DECK_SIZE; i++)
				op[i] = (i == hand) ? 0.0 : 1.0 / (DECK_SIZE - 1);
			sum += BestResponse(brplayer, hand, op);
		}
		return sum / DECK_SIZE;
	}
	virtual double BestResponse(int brplayer, int hand, const vector<double>& op) override {
		int n_children = children.size();
		if (player == brplayer) {
			double bestev = -1000000000000000000;
			//int besta = -1;
			for (int i = 0; i < n_children; i++) {
				double ev = children[i]->BestResponse(brplayer, hand, op);
				if (ev > bestev) {
					bestev = ev;
					//besta = i;
				}
			}
			//for (double x : op) std::cout << x << ' ';
			//cout << std::endl;
			//std::cout << brplayer << ' ' << hand << ' ' << bestev << std::endl;
			return bestev;
		}
		else {
			double ev = 0;
			for (int i = 0; i < n_children; i++) {
				vector<double> newop(op.size());
				for (int h = 0; h < DECK_SIZE; h++) {
					vector<double> s = GetNormalizedAverageStrategy(h);
					newop[h] = s[i] * op[h];
				}
				ev += children[i]->BestResponse(brplayer, hand, newop);
			}
			return ev;
		}
	}
	void PrintStrategy(int* hands, int depth, string history) {
		vector<double> s = GetNormalizedAverageStrategy(hands[player]);
		for (int i = 0; i < depth; i++) std::cout << ' ';
		std::cout << history << ' ' << s[0] << ' ' << s[1] << std::endl;
		for (int i = 0; i < (int)children.size(); i++) {
			children[i]->PrintStrategy(hands, depth + 1, history + (i == 0 ? "b" : "x"));
		}
	}

	GameTreeNode* proceed(Action action) {
		switch (action.action_type)
		{
		case FOLD_ACTION_TYPE:
		{
			int delta = STARTING_STACK - this->stacks[player];
			return new Fold(delta);
		}
		case CALL_ACTION_TYPE:
		{
			if (this->button == 0)  // sb calls bb
			{
				return new RoundState(1, 0, (array<int, 2>) { BIG_BLIND, BIG_BLIND },
					(array<int, 2>) {
					STARTING_STACK - BIG_BLIND, STARTING_STACK - BIG_BLIND
				},
					this->hands, this->deck, this);
			}
			// both players acted
			array<int, 2> new_pips = this->pips;
			array<int, 2> new_stacks = this->stacks;
			int contribution = new_pips[1 - active] - new_pips[active];
			new_stacks[active] -= contribution;
			new_pips[active] += contribution;
			RoundState* state = new RoundState(this->button + 1, this->street, new_pips, new_stacks,
				this->hands, this->deck, this);
			return state->proceed_street();
		}
		case CHECK_ACTION_TYPE:
		{
			if (((this->street == 0) & (this->button > 0)) | (this->button > 1))  // both players acted
			{
				return this->proceed_street();
			}
			// let opponent act
			return new RoundState(this->button + 1, this->street, this->pips, this->stacks, this->hands, this->deck, this);
		}
		default:  // RAISE_ACTION_TYPE
		{
			array<int, 2> new_pips = this->pips;
			array<int, 2> new_stacks = this->stacks;
			int contribution = action.amount - new_pips[active];
			new_stacks[active] -= contribution;
			new_pips[active] += contribution;
			return new RoundState(this->button + 1, this->street, new_pips, new_stacks, this->hands, this->deck, this);
		}
		}
	}
};
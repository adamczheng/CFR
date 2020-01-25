#include <vector>
#include <random>
#include <chrono>
#include <cassert>
#include <array>
#include <mutex>;
#include "states.hpp"
#include "Buckets.cpp"
#include <iomanip>
#include <unordered_map>
using std::vector;
using std::array;
class InfoSet {
public:
	// index i refers to child i
	mutex m;
	vector<double> regret;
	vector<double> cumulativeStrategy;
	int cnt;
	InfoSet() {
		cnt = 0;
	}
	InfoSet(int sz) {
		regret.assign(sz, 0.0);
		cumulativeStrategy.assign(sz, 0.0);
		cnt = 0;
	}
	void update_regrets(const vector<double>& u, double ev) {
		lock_guard<mutex> mg(m);
		for (int i = 0; i < (int)regret.size(); i++) {
			regret[i] += u[i] - ev;
		}
	}
	void update_cumulative_strategy(const vector<double>& s, double p, double op, int iter) {
		lock_guard<mutex> mg(m);
		for (int i = 0; i < (int)regret.size(); i++) {
			cumulativeStrategy[i] += (p * s[i] / op) * iter;
		}
	}
};
class CFR {
	std::mt19937 rng;
	unordered_map<int, InfoSet*> infoset[6][200];
public:
	unordered_map<int, int> pot_index[6][6];
	Buckets* Bucketer;
	int iset_cnt;
	CFR() {
		iset_cnt = 0;
		rng.seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());
		Bucketer = new Buckets();
	}
	int SampleStrategy(const vector<double>& s) {
		std::uniform_real_distribution<double> distribution(0, 1);
		double r = distribution(rng);
		double acc = 0;
		for (int i = 0; i < (int)s.size(); i++) {
			acc += s[i];
			if (r < acc) return i;
		}
		assert(0);
	}

	int pot_odds_bucket(double odds) {
		if (odds * 6 < 1) return 0;
		if (odds < 0.25) return 1;
		if (odds * 3 < 1) return 2;
		if (odds < 0.4) return 3;
		if (odds * 9 < 4) return 4;
		return 5;
	}

	double TrainExternalSampling(int iter, State* state, int trainplayer, array<array<int, 2>, 2> hands, array<int, 5> board, double p, double op, int who_won, bool folded = false) {
		if (state == NULL) {
			cerr << "WTF?" << endl;
		}
		assert(state->is_decision == (state->children.size() > 0));
		if (state->is_decision) {
			RoundState* round_state = (RoundState*)state;
			int player = (round_state->button) % 2;
			int active = player;
			int my_pip = round_state->pips[active];  // the number of chips you have contributed to the pot this round of betting
			int opp_pip = round_state->pips[1 - active];  // the number of chips your opponent has contributed to the pot this round of betting
			int my_stack = round_state->stacks[active];  // the number of chips you have remaining
			int opp_stack = round_state->stacks[1 - active];  // the number of chips your opponent has remaining
			int continue_cost = opp_pip - my_pip;  // the number of chips needed to stay in the pot
			int my_contribution = STARTING_STACK - my_stack;  // the number of chips you have contributed to the pot
			int opp_contribution = STARTING_STACK - opp_stack;  // the number of chips your opponent has contributed to the pot
			int pot = 2 * opp_contribution;
			int rounded_pot_odds = pot_odds_bucket(1.0 * continue_cost / pot);
			int card_abs = Bucketer->GetBucket(round_state->street, hands[player], board);
			int potid = pot_index[round_state->button][rounded_pot_odds][pot];
			InfoSet*& iset = infoset[round_state->street]
				[card_abs]
			[potid];

			if (iset == NULL) {
				iset = new InfoSet((int)round_state->children.size());
				iset_cnt++;
			}
			iset->cnt++;
			if (player == trainplayer) {
				vector<double> s = GetStrategy(iset);
				int n_children = round_state->children.size();
				iset->update_cumulative_strategy(s, p, op, iter);
				
				vector<double> u(n_children);
				double ev = 0;
				for (int i = 0; i < n_children; i++) {
					double f = (round_state->children[i].second.first == 'F');
					if (round_state->children[i].first == NULL) {
						cerr << "dafuq" << endl;
					}
					u[i] = TrainExternalSampling(iter, round_state->children[i].first, trainplayer, hands, board, p * s[i], op, who_won, f);
					ev += u[i] * s[i];
				}
				iset->update_regrets(u, ev);
					
				return ev;
			}
			else {
				vector<double> s = GetStrategy(iset);
				int a = SampleStrategy(s);
				bool f = (round_state->children[a].second.first == 'F');
				if (round_state->children[a].first == NULL) {
					cerr << "dafuq2"  <<' ' << a <<' ' <<s.size() << ' ' << round_state->children.size()<< endl;
					cerr << round_state->street << ' ' << my_pip << ' ' << opp_pip << ' ' << pot << endl;
				}
				return TrainExternalSampling(iter, round_state->children[a].first, trainplayer, hands, board, p, op * s[a], who_won, f);
			}
		}
		else {
			TerminalState* terminal_state = (TerminalState*)state;
			int pos_delta = terminal_state->deltas[0];
			if (pos_delta < 0) pos_delta = -pos_delta;
			if (folded) {
				return terminal_state->deltas[trainplayer];
			}
			if (who_won == 2)
				return 0;
			if (who_won == trainplayer)
				return pos_delta;
			return -pos_delta;
		}
	}

	vector<double> GetStrategy(InfoSet* iset) {
		int n_children = iset->regret.size();

		vector<double> s(n_children);
		double psum = 0;
		for (int i = 0; i < n_children; i++)
			if (iset->regret[i] > 0)
				psum += iset->regret[i];

		if (psum > 0) {
			for (int i = 0; i < n_children; i++)
				s[i] = (iset->regret[i] > 0) ? iset->regret[i] / psum : 0.0;
		}
		else {
			for (int i = 0; i < n_children; i++)
				s[i] = 1.0 / n_children;
		}

		return s;
	}
	vector<double> GetNormalizedAverageStrategy(InfoSet* iset) {
		//const double THRESHOLD = 0.01; // make actions with p < THRESHOLD never happen
		int n_children = iset->regret.size();
		vector<double> s(n_children);
		double sum = 0;
		for (int i = 0; i < n_children; i++)
			sum += iset->cumulativeStrategy[i] > 0 ? iset->cumulativeStrategy[i] : 0.0;
		if (sum > 0)
			for (int i = 0; i < n_children; i++) {
				if (iset->cumulativeStrategy[i] > 0) s[i] = iset->cumulativeStrategy[i] / sum;
				else s[i] = 0;
			}
		else
			for (int i = 0; i < n_children; i++)
				s[i] = 1.0 / n_children;
		return s;
	}
	const static int DECK_SIZE = 52;
	const static int NUM_BET_SIZES = 4;
	const static int MAX_ACTIONS_PER_STREET = 4;
	const static int NUM_RAISE_SIZES = 2;
	const array<double, NUM_BET_SIZES> BET_SIZES = { 0.5, 1.0, 2.0, 999 };
	const array<double, NUM_RAISE_SIZES> RAISE_SIZES = { 1.0, 999 };
	void dump_strategy(State* state) {
		if (state->is_decision) {
			RoundState* round_state = (RoundState*)state;
			int active = (round_state->button) % 2;
			int legal_action_mask = round_state->legal_actions();
			int my_pip = round_state->pips[active];  // the number of chips you have contributed to the pot this round of betting
			int opp_pip = round_state->pips[1 - active];  // the number of chips your opponent has contributed to the pot this round of betting
			int my_stack = round_state->stacks[active];  // the number of chips you have remaining
			int opp_stack = round_state->stacks[1 - active];  // the number of chips your opponent has remaining
			int continue_cost = opp_pip - my_pip;  // the number of chips needed to stay in the pot
			int my_contribution = STARTING_STACK - my_stack;  // the number of chips you have contributed to the pot
			int opp_contribution = STARTING_STACK - opp_stack;  // the number of chips your opponent has contributed to the pot
			int pot = 2 * opp_contribution;
			int child_id = 0;
			for (int i = 0; i < 200; i++) {
				int rounded_pot_odds = pot_odds_bucket(1.0 * continue_cost / pot);
				InfoSet*& iset = infoset[round_state->street]
					[i]
				[pot_index[round_state->button][rounded_pot_odds][pot]];
				if (iset == NULL) continue;
				if (pot == 400 && my_contribution == opp_contribution) continue;
				cout << round_state->street << ' ' << i << ' ';
				cout /*<< active << ' '*/<< round_state->button <<' ' << pot << ' ' << pot_odds_bucket(1.0 * continue_cost / pot) << ": ";
				vector<double> s = GetNormalizedAverageStrategy(iset);
				for (int j = 0; j < (int)s.size(); j++)
					cout << fixed << setprecision(4) << round_state->children[j].second.first <<' ' << round_state->children[j].second.second<< ' '<< s[j] << ' ';
				//cout << iset->cnt;
				cout << endl;
			}
			for (int i = 0; i < (int)state->children.size(); i++) {
				dump_strategy(state->children[i].first);
			}
			return;
		}
	}
};
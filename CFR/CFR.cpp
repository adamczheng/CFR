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
		//cerr << "update regret" << endl;
		lock_guard<mutex> mg(m);
		for (int i = 0; i < (int)regret.size(); i++) {
			regret[i] += u[i] - ev;
		}
	}
	void update_cumulative_strategy(const vector<double>& s, double p, double op, int iter) {
		//cerr << "updating cumulative_strategy" << endl;
		lock_guard<mutex> mg(m);
		for (int i = 0; i < (int)regret.size(); i++) {
			cumulativeStrategy[i] += p * s[i] / op;
			cumulativeStrategy[i] *= 1.0 * iter / (iter + 1);
		}
	}
};
class CFR {
	std::mt19937 rng;
	unordered_map<int, InfoSet*> infoset[6][200];
public:
	map<pair<int, pair<int, int> >, int> pot_index[2];
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

	double TrainExternalSampling(int iter, State* state, int trainplayer, array<array<int, 2>, 2> hands, array<int, 5> board, double p, double op, int who_won, double folded = false) {
		//cerr << iter << endl;
		if (state->is_decision) {
			RoundState* round_state = (RoundState*)state;
			//assert(round_state->who_won == state->who_won);
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
			InfoSet*& iset = infoset[round_state->street]
				/*[hands[player][0]]*/[Bucketer->GetBucket(round_state->street, hands[player], board)]
			[pot_index[player][{pot, { round_state->pips[0], round_state->pips[1] }}]];
			//[(long)pot_index[pot][round_state->pips[0]][round_state->pips[1]]];

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
					//round_state->children[i]->who_won = who_won;
					bool folded = false;
					if (!round_state->children[i]->is_decision) {
						RoundState* ps = (RoundState*)((TerminalState*)(round_state->children[i]))->previous_state;
						if (ps->pips[0] != ps->pips[1]) {
							folded = true;
							//assert(i == n_children - 1);
						}
					}
					u[i] = TrainExternalSampling(iter, round_state->children[i], trainplayer, hands, board, p * s[i], op, who_won, true);
					ev += u[i] * s[i];
				}
				iset->update_regrets(u, ev);
					
				return ev;
			}
			else {
				vector<double> s = GetStrategy(iset);
				int a = SampleStrategy(s);
				//round_state->children[a]->who_won = who_won;
				bool folded = false;
				if (!round_state->children[a]->is_decision) {
					RoundState* ps = (RoundState*)((TerminalState*)(round_state->children[a]))->previous_state;
					if (ps->pips[0] != ps->pips[1]) {
						folded = true;
						//assert(a == (int)s.size() - 1);
					}
				}
				return TrainExternalSampling(iter, round_state->children[a], trainplayer, hands, board, p, op * s[a], who_won, folded);
			}
		}
		else {
			TerminalState* terminal_state = (TerminalState*)state;
			int pos_delta = terminal_state->deltas[0];
			if (pos_delta < 0) pos_delta = -pos_delta;
			//assert(who_won == terminal_state->who_won);
			//assert(terminal_state->who_won == terminal_state->previous_state->who_won);
			if (folded) {
				return terminal_state->deltas[trainplayer];
				//if (((((RoundState*)terminal_state->previous_state)->button) % 2) == trainplayer) {
					//assert(terminal_state->deltas[trainplayer] == pos_delta);
					//return pos_delta;
				//}
				//assert(terminal_state->deltas[trainplayer] == -pos_delta);
				//return -pos_delta;
				//assert(terminal_state->deltas[(((RoundState*)terminal_state->previous_state)->button) % 2] < 0);
			}
			else {
				//assert((hands[0][0] < hands[1][0]) == (terminal_state->deltas[0] < 0));
				//assert((hands[0][0] > hands[1][0]) == (terminal_state->deltas[0] > 0));
			}
			if (who_won == 2)
				return 0;
			if (who_won == trainplayer)
				return pos_delta;
			return -pos_delta;
			//return terminal_state->deltas[trainplayer];
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
		//bool flag = false;
		double sum = 0;
		for (int i = 0; i < n_children; i++)
			sum += iset->cumulativeStrategy[i] > 0 ? iset->cumulativeStrategy[i] : 0.0;
		if (sum > 0)
			for (int i = 0; i < n_children; i++) {
				if (iset->cumulativeStrategy[i] > 0 /*&& iset->cumulativeStrategy[i] / sum >= THRESHOLD*/) {
					//flag = true;
					s[i] = iset->cumulativeStrategy[i] / sum;
				}
				else {
					s[i] = 0;
				}
			}
		else
			for (int i = 0; i < n_children; i++)
				s[i] = 1.0 / n_children;
		/*if (flag) {
			sum = 0;
			for (int i = 0; i < n_children; i++)
				sum += s[i];
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
		}*/
		return s;
	}
	const static int DECK_SIZE = 52;
	const static int NUM_BET_SIZES = 4;
	const static int MAX_ACTIONS_PER_STREET = 4;
	const static int NUM_RAISE_SIZES = 2;
	const array<double, NUM_BET_SIZES> BET_SIZES = { 0.5, 1.0, 2.0, 999 };
	const array<double, NUM_RAISE_SIZES> RAISE_SIZES = { 1.0, 999 };
	void dump_strategy(State* state, vector<pair<char, int> > history) {
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
				InfoSet*& iset = infoset[round_state->street]
					[i]
				[pot_index[active][{pot, { round_state->pips[0], round_state->pips[1] }}]];
				if (iset == NULL) continue;
				if (pot == 400) continue;
				cout << round_state->street << ' ' << i << ' ';
				cout << active << ' ' << pot << ' ' << round_state->pips[0] << ' ' << round_state->pips[1] << ": ";
				vector<double> s = GetNormalizedAverageStrategy(iset);
				for (int j = 0; j < (int)s.size(); j++)
					cout << fixed << setprecision(4) << s[j] << ' ';
				cout << iset->cnt;
				cout << endl;
			}

			if ((legal_action_mask & RAISE_ACTION_TYPE) && round_state->button < MAX_ACTIONS_PER_STREET) {
				int min_raise = round_state->raise_bounds()[0];
				int max_raise = round_state->raise_bounds()[1];
				bool is_bet = (continue_cost == 0);
				if (is_bet) {
					for (int i = 0; i < NUM_BET_SIZES; i++) {
						int delta = continue_cost + (int)(BET_SIZES[i] * pot);
						int sizing = my_pip + delta;
						if (min_raise <= sizing && sizing < max_raise) {
							dump_strategy(state->children[child_id++], history);
						}
						if (BET_SIZES[i] > 990) {
							dump_strategy(state->children[child_id++], history);
						}
					}
				}
				else {
					for (int i = 0; i < NUM_RAISE_SIZES; i++) {
						int delta = continue_cost + (int)(RAISE_SIZES[i] * pot);
						int sizing = my_pip + delta;
						if (min_raise <= sizing && sizing < max_raise) {
							dump_strategy(state->children[child_id++], history);
						}
						if (RAISE_SIZES[i] > 990) {
							dump_strategy(state->children[child_id++], history);
						}
					}
				}
			}
			if (legal_action_mask & CHECK_ACTION_TYPE) {
				dump_strategy(state->children[child_id++], history);
			}
			else {
				//assert(legal_action_mask & CALL_ACTION_TYPE);
				//assert(legal_action_mask & FOLD_ACTION_TYPE);
				dump_strategy(state->children[child_id++], history);
				dump_strategy(state->children[child_id++], history);
			}
			return;
		}
	}
};
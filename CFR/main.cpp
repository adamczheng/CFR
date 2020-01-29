#include <bits/stdc++.h>
#include <thread>
using namespace std;
#include "states.hpp"
#include "actions.hpp"
#include "CFR.cpp"
#include "LUT.h"

using namespace std;

const int DECK_SIZE = 52;
const int MAX_ACTIONS_PER_STREET = 999;
const int NUM_BET_SIZES = 4;
const int NUM_RAISE_SIZES = 4;
const array<double, NUM_BET_SIZES> BET_SIZES = { 0.5, 1.0, 2.0, 999 };
const array<double, NUM_RAISE_SIZES> RAISE_SIZES = { 0.5, 1.0, 2.0, 999 };
set<pair<int, pair<int, int> > > n_sets[6];
int pot_odds_bucket(double odds) {//
	if (odds * 6 < 1) return 0;
	if (odds < 0.25) return 1;
	if (odds * 3 < 1) return 2;
	if (odds < 0.4) return 3;
	if (odds * 9 < 4) return 4;
	return 5;
}
void build_tree(State*& state) {
	assert(state->is_decision);
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
	n_sets[round_state->street].insert({ pot, {round_state->pips[0], round_state->pips[1]} });
	int init_button = (round_state->street > 0) ? 1 : 0;
	if ((legal_action_mask & RAISE_ACTION_TYPE) && round_state->button < init_button + MAX_ACTIONS_PER_STREET) {
		int min_raise = round_state->raise_bounds()[0];
		int max_raise = round_state->raise_bounds()[1];
		bool is_bet = (continue_cost == 0);
		if (is_bet) {//
			for (int i = 0; i < NUM_BET_SIZES; i++) {
				int delta = continue_cost + (int)round(BET_SIZES[i] * pot);
				int sizing = my_pip + delta;
				if (min_raise <= sizing && sizing < max_raise) {
					state->children.push_back({ round_state->proceed(RaiseAction(sizing)), {'B', i} });
				}
				if (BET_SIZES[i] > 990) {
					state->children.push_back({ round_state->proceed(RaiseAction(max_raise)), {'B', i} });
				}
			}
		}
		else {
			for (int i = 0; i < NUM_RAISE_SIZES; i++) {
				int delta = continue_cost + (int)round(RAISE_SIZES[i] * pot);
				int sizing = my_pip + delta;
				if (min_raise <= sizing && sizing < max_raise) {
					state->children.push_back({ round_state->proceed(RaiseAction(sizing)), {'R', i} });
				}
				if (RAISE_SIZES[i] > 990) {//
					state->children.push_back({ round_state->proceed(RaiseAction(max_raise)), {'R', i} });
				}
			}
		}//
	}
	/*if (legal_action_mask & RAISE_ACTION_TYPE) {
		state->children.push_back({ round_state->proceed(RaiseAction(round_state->raise_bounds()[1])), {'R', NUM_RAISE_SIZES - 1} });
	}*/
	if (legal_action_mask & CHECK_ACTION_TYPE) {
		state->children.push_back({ round_state->proceed(CheckAction()), {'X', 0} });
	}
	else {
		assert(legal_action_mask & CALL_ACTION_TYPE);
		assert(legal_action_mask & FOLD_ACTION_TYPE);
		state->children.push_back({ round_state->proceed(CallAction()), {'C', 0} });
		if (pot_odds_bucket(1.0*continue_cost/pot) != 0)
			state->children.push_back({ round_state->proceed(FoldAction()), {'F', 0} });
	}
	for (pair<State*,pair<char,int> > s : state->children) {
		if (s.first->is_decision) {
			build_tree(s.first);
		}
	}
	return;
}



int main() {
	State* root = new RoundState(0, 0, array<int, 2>({ 1, 2 }), array<int, 2>({ 199, 198 }));
	build_tree(root);
	cerr << n_sets[0].size() << ' ' << n_sets[3].size() << ' ' << n_sets[4].size() << ' ' << n_sets[5].size() << endl;
	CFR cfr;

	int i = 0;
	for (int street = 0; street <= 5; street++) {
		if (street == 1 || street == 2) continue;
		for (pair<int, pair<int, int> > p : n_sets[street]) {
			double pot_odds = 1.0 * abs(p.second.second - p.second.first) / p.first;
			for (int j = 0; j < 15; j++) {//
				cfr.pot_index[j][pot_odds_bucket(pot_odds)][p.first] = i++;
			}
		}
	}
	
	int training_iter = 0;
	int iter = 1;
	while (1) {
		cerr << "training iteration #" << training_iter << " starting" << endl;
		string fname = "dump" + to_string((int)(training_iter%10)) + ".txt";
		training_iter++;
		freopen(fname.c_str(), "w", stdout);
		const int NUM_WORKERS = 96;
		thread workers[NUM_WORKERS];//
		for (int ii = 0; ii < NUM_WORKERS; ii++) {
			workers[ii] = (thread([&iter, &cfr, &root, ii]() {
				double num_hours = 0.1;
				auto TIME = clock();
				int target = iter + 10000000;
				for (; iter <= target; iter += NUM_WORKERS) {
					if (1.0 * (clock() - TIME) / CLOCKS_PER_SEC > 3600.0 * NUM_WORKERS * num_hours) {
						break;
					}
					//if ((iter + ii) % 100000 == 0) cerr << iter + ii << endl;
					mt19937 rng(std::chrono::high_resolution_clock::now().time_since_epoch().count());
					uniform_int_distribution<int> distribution(0, 51);
					bitset<52> bs;
					array<array<int, 2>, 2> hands;
					array<int, 5> board;
					for (int a = 0; a < 2; a++) {
						for (int b = 0; b < 2; b++) {
							int x;
							do {
								x = distribution(rng);
							} while (bs[x]);
							bs[x] = 1;
							hands[a][b] = x;
						}
					}
					for (int a = 0; a < 5; a++) {
						int x;
						do {
							x = distribution(rng);
						} while (bs[x]);
						bs[x] = 1;
						board[a] = x;
					}
					array<int, 7> cards = { board[0], board[1], board[2], board[3], board[4], hands[0][0], hands[0][1] };
					int player0strength = cfr.Bucketer->hr->GetHandValue(cards);
					cards[5] = hands[1][0];
					cards[6] = hands[1][1];
					int player1strength = cfr.Bucketer->hr->GetHandValue(cards);
					int who_won = 2; // 2 = tie
					if (player0strength > player1strength) who_won = 0;
					else if (player1strength > player0strength) who_won = 1;

					cfr.TrainExternalSampling(iter + ii, root, 0, hands, board, 1, 1, who_won);
					cfr.TrainExternalSampling(iter + ii, root, 1, hands, board, 1, 1, who_won);
				}
			}));
		}
		for_each(workers, workers + NUM_WORKERS, [](thread& t) {
			t.join();
		});
		cerr << "training iteration done!" << endl;
		cout << cfr.iset_cnt << endl;
		for (int z = 0; z < 15; z++) {
			for (int zz = 0; zz < 200; zz++) {
				cfr.seeninfoset[z][zz].clear();
			}
		}
		cfr.dump_strategy(root);
	}
	return 0;
}
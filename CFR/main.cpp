#include <bits/stdc++.h>
#include "Decision.cpp"
#include "Showdown.cpp"
#include "Fold.cpp"
#include "states.hpp"
#include "actions.hpp"

using namespace std;

const int DECK_SIZE = 52;
const int NUM_BET_SIZES = 4;
const int NUM_RAISE_SIZES = 2;
const array<double, NUM_BET_SIZES> BET_SIZES = { 0.5, 1.0, 2.0, 999 };
const array<double, NUM_RAISE_SIZES> RAISE_SIZES = { 1.0, 999 };
mt19937 rng;
set<pair<int,pair<int,int> > > n_sets[6];
void build_tree(State*& state, int active) {
	if (state->is_decision) {
		RoundState* round_state = (RoundState*)state;
		int legal_action_mask = round_state->legal_actions();
		int my_pip = round_state->pips[active];  // the number of chips you have contributed to the pot this round of betting
		int opp_pip = round_state->pips[1-active];  // the number of chips your opponent has contributed to the pot this round of betting
		int my_stack = round_state->stacks[active];  // the number of chips you have remaining
		int opp_stack = round_state->stacks[1-active];  // the number of chips your opponent has remaining
		int continue_cost = opp_pip - my_pip;  // the number of chips needed to stay in the pot
		int my_contribution = STARTING_STACK - my_stack;  // the number of chips you have contributed to the pot
		int opp_contribution = STARTING_STACK - opp_stack;  // the number of chips your opponent has contributed to the pot
		int pot = 2 * opp_contribution;
		n_sets[round_state->street].insert({ pot, {my_pip, opp_pip} });
		if ((legal_action_mask & RAISE_ACTION_TYPE) && round_state->button < 4) {
			int min_raise = round_state->raise_bounds()[0];
			int max_raise = round_state->raise_bounds()[1];
			bool is_bet = (continue_cost == 0);
			if (is_bet) {
				for (int i = 0; i < NUM_BET_SIZES; i++) {
					int delta = continue_cost + (int)(BET_SIZES[i] * pot);
					int sizing = my_pip + delta;
					if (min_raise <= sizing && sizing < max_raise) {
						state->children.push_back(round_state->proceed(RaiseAction(sizing)));
					}
					if (BET_SIZES[i] > 990) {
						state->children.push_back(round_state->proceed(RaiseAction(max_raise)));
					}
				}
			}
			else {
				for (int i = 0; i < NUM_RAISE_SIZES; i++) {
					int delta = continue_cost + (int)(RAISE_SIZES[i] * pot);
					int sizing = my_pip + delta;
					if (min_raise <= sizing && sizing < max_raise) {
						state->children.push_back(round_state->proceed(RaiseAction(sizing)));
					}
					if (RAISE_SIZES[i] > 990) {
						state->children.push_back(round_state->proceed(RaiseAction(max_raise)));
					}
				}
			}
		}
		if (legal_action_mask & CHECK_ACTION_TYPE) {
			state->children.push_back(round_state->proceed(CheckAction()));
		}
		else {
			assert(legal_action_mask & CALL_ACTION_TYPE);
			assert(legal_action_mask & FOLD_ACTION_TYPE);
			state->children.push_back(round_state->proceed(CallAction()));
			state->children.push_back(round_state->proceed(FoldAction()));
		}
		for (State* s : state->children) {
			if (s->is_decision) {
				build_tree(s, (((RoundState*)(s))->button) % 2);
				//cout << chsz << endl;

			}
			else {
				//n_sets[round_state->street][];
			}
		}
		return;
		//cout << round_state->street << ' ' << round_state->button << ' ' << round_state->children.size() << endl;
		
	}
	assert(0);
}

// Set up a 7-card poker hand, perform the lookup into the table, and
// extract the category (full house, two pair, flush, etc.) and rank
// (within that category) of the hand. These numbers can be used to
// compare the hand to other hands.
inline bool exists_test1(const std::string& name) {
	if (FILE* file = fopen(name.c_str(), "rb")) {
		fclose(file);
		return true;
	}
	else {
		return false;
	}
}
int main() {
	rng.seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());
	// Fold(N): player 0 wins N, player 1 loses N
	// Showdown(N): better hand wins N, worse hand loses N
	// ante 1
	array<array<int, 2>, 2> tmp;
	tmp[0][0] = tmp[0][1] = tmp[1][0] = tmp[1][1] = 0;
	State* root = new RoundState(0, 0,
		array<int,2>({ 1, 2 }),
		array<int,2>({ 199, 198 }),
		tmp,
		array<int,5>({ 0, 0, 0, 0, 0 }), NULL);
	build_tree(root, 0);
	cout << n_sets[0].size() << ' ' << n_sets[3].size() << ' ' << n_sets[4].size() << ' ' << n_sets[5].size() << endl;
	/*Decision* root = new Decision(0, {
		// p0 bets 1
		new Decision(1, {
			// p1 calls
			new Showdown(2),
			// p1 folds
			new Fold(1)
		}),
		// p0 checks
		new Decision(1, {
			// p1 bets 1
			new Decision(0, {
				// p0 calls
				new Showdown(2),
				// p0 folds
				new Fold(-1)
			}),
			// p1 checks
			new Showdown(1)
		})
		});*/
	/*for (int iters = 0; iters < 10000000; iters++) {
		auto hands = DealHands();
		root->TrainExternalSampling(0, hands, 1, 1);
		root->TrainExternalSampling(1, hands, 1, 1);
		if (iters % 1000000 == 0) {
			cout << "iteration #" << iters << " exploitability" << endl;
			double br0 = root->BestResponse(0);
			double br1 = root->BestResponse(1);
			cout << fixed << setprecision(10) << br0 << ' ' << br1 << ' ' << br0 + br1 << endl;
		}
	}
	for (int a = 0; a < 3; a++) {
		for (int b = 0; b < 3; b++) {
			if (a != b) {
				cout << a << ' ' << b << endl;
				int hands[2] = { a, b };
				root->PrintStrategy(hands, 0, "");
			}
		}
	}*/
	return 0;
}
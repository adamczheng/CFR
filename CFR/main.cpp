#include <bits/stdc++.h>
#include "Decision.cpp"
#include "Showdown.cpp"
#include "Fold.cpp"

using namespace std;

const int DECK_SIZE = 3;
mt19937 rng;

int* DealHands() {
	static int hands[2];
	uniform_int_distribution<int> distribution(0, DECK_SIZE - 1);
	hands[0] = distribution(rng);
	do {
		hands[1] = distribution(rng);
	} while (hands[0] == hands[1]);
	return hands;
}

int main() {
	rng.seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());
	// Fold(N): player 0 wins N, player 1 loses N
	// Showdown(N): better hand wins N, worse hand loses N
	// ante 1
	Decision* root = new Decision(0, {
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
		});
	for (int iters = 0; iters < 10000000; iters++) {
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
	}
	return 0;
}
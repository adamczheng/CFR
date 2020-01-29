//extern "C" {
//#include "hand_index.h"
//}
//#include "LUT.h"
//#include <bits/stdc++.h>
//using namespace std;
//
//int HR[32487834];
//int GetHandValue(array<int, 7> cards) {
//	int p = HR[53 + cards[0] + 1];
//	p = HR[p + cards[1] + 1];
//	p = HR[p + cards[2] + 1];
//	p = HR[p + cards[3] + 1];
//	p = HR[p + cards[4] + 1];
//	p = HR[p + cards[5] + 1];
//	return HR[p + cards[6] + 1];
//}
//class Buckets {
//	string pf_cluster[8] = { "23s,24s,25s,26s,27s,34s,35s,36s,37s,45s,46s,32o,43o,42o,54o,53o,52o,65o,64o,63o,62o,74o,73o,72o,83o,82o","28s,29s,2Ts,38s,39s,47s,48s,49s,75o,85o,84o,95o,94o,93o,92o,T5o,T4o,T3o,T2o,J3o,J2o","3Ts,4Ts,56s,57s,58s,59s,5Ts,67s,68s,69s,6Ts,78s,79s,89s,67o,68o,69o,6To,78o,79o,7To,89o,8To","22,J2s,J3s,J4s,J5s,J6s,Q2s,Q3s,Q4s,Q5s,K2s,J4o,J5o,J6o,J7o,Q2o,Q3o,Q4o,Q5o,Q6o,Q7o,K2o,K3o,K4o","6Qs,7Ts,7Js,7Qs,8Ts,8Js,8Qs,9Ts,9Js,9Qs,TJs,T9o,J8o,J9o,JTo,Q8o,Q9o,QTo,QJo","33,44,55,K3s,K4s,K5s,K6s,K7s,K8s,A2s,A3s,A4s,A5s,A6s,K5o,K6o,K7o,K8o,K9o,A2o,A3o,A4o,A5o,A6o,A7o,A8o","66,77,QTs,QJs,K9s,KTs,KJs,KQs,A7s,A8s,A9s,ATs,AJs,AQs,AKs,KTo,KJo,KQo,A9o,ATo,AJo,AQo,AKo","88,99,TT,JJ,QQ,KK,AA" };
//	vector<array<int, 2> > preflop_cluster[8];
//	// AKo or AKs or AA to a vector of card indices (0-indexed)
//	vector<array<int, 2> > convert(string s) {
//		string ranks = "23456789TJQKA";
//		string suits = "shcd";
//		map<char, int> rank_rev, suit_rev;
//		int i = 0;
//		for (char c : ranks) rank_rev[c] = i++;
//		i = 0;
//		for (char c : suits) suit_rev[c] = i++;
//		vector<array<int, 2> > res;
//		if (s.size() == 2) {
//			assert(s[0] == s[1]);
//			// pocket pairs
//			int f = 4 * rank_rev[s[0]];
//			for (int i = 0; i < 4; i++)
//				for (int j = i + 1; j < 4; j++)
//					res.push_back(array<int, 2>{f + i, f + j});
//			return res;
//		}
//		assert(s.size() == 3);
//		int r1 = 4 * rank_rev[s[0]];
//		int r2 = 4 * rank_rev[s[1]];
//		if (s.back() == 's') {
//			// suited
//			for (int i = 0; i < 4; i++)
//				res.push_back(array<int, 2>{r1 + i, r2 + i});
//		}
//		else {
//			assert(s.back() == 'o');
//			// unsuited
//			for (int i = 0; i < 4; i++) {
//				for (int j = 0; j < 4; j++) {
//					if (i == j) continue;
//					res.push_back(array<int, 2>{r1 + i, r2 + j});
//				}
//			}
//		}
//		return res;
//	}
//	void split_clusters() {
//		for (int i = 0; i < 8; i++) {
//			vector<string> split_cluster;
//			istringstream ss(pf_cluster[i]);
//			string token;
//			while (getline(ss, token, ',')) {
//				split_cluster.push_back(token);
//				//cout << token << endl;
//			}
//			for (string s : split_cluster) {
//				vector<array<int, 2> > tmp = convert(s);
//				for (array<int, 2> x : tmp) preflop_cluster[i].push_back(x);
//			}
//		}
//	}
//	bool load_lookup(const char* filename) {
//		memset(HR, 0, sizeof(HR));
//		FILE* fin = fopen("handranks.dat", "rb");
//		size_t bytesread = fread(HR, sizeof(HR), 1, fin);
//		fclose(fin);
//		return true;
//	}
//	ifstream f_in, t_in, r_in;
//	vector<vector<double> > flop_centers, turn_centers, river_centers;
//public:
//	hand_indexer_t indexer[4];
//	//CLutFlop* flop;
//	//CLutTurn* turn;
//	CLutRiver* river;
//	Buckets() {
//		load_lookup("handranks.dat");
//		//flop = new CLutFlop();
//		//turn = new CLutTurn();
//		river = new CLutRiver();
//		river->read_suitfile("river_suit.dat");
//		river->load("river_lut.dat");
//		//flop->read_suitfile("flop_suit.dat");
//		//turn->read_suitfile("turn_suit.dat");
//		//flop->load("flop_lut.dat");
//		//turn->load("turn_lut.dat");
//		f_in.open("flopcenters.txt");
//		for (int i = 0; i < 100; i++) {
//			vector<double> p(51);
//			for (int j = 0; j < 51; j++) {
//				f_in >> p[j];
//			}
//			flop_centers.push_back(p);
//		}
//		f_in.close();
//		t_in.open("turncenters.txt");
//		for (int i = 0; i < 100; i++) {
//			vector<double> p(51);
//			for (int j = 0; j < 51; j++) {
//				t_in >> p[j];
//			}
//			turn_centers.push_back(p);
//		}
//		t_in.close();
//		r_in.open("rivercenters.txt");
//		for (int i = 0; i < 150; i++) {
//			vector<double> p(8);
//			for (int j = 0; j < 8; j++) {
//				r_in >> p[j];
//			}
//			river_centers.push_back(p);
//		}
//		r_in.close();
//		split_clusters();
//	}
//	static int GetBucketCount(int street) {
//		if (street == 0) {
//			return 169;
//		}
//		else if (street == 3) {
//			return 100;
//		}
//		else if (street == 4) {
//			return 100;
//		}
//		else {
//			// street == 5
//			return 100;
//		}
//	}
//	vector<double> GetHistogram(int street, array<int, 2> hand, array<int, 5> board) {
//		assert(street == 3 || street == 4);
//		bitset<52> used;
//		used[hand[0]] = 1;
//		used[hand[1]] = 1;
//		for (int i = 0; i < street; i++)
//			used[board[i]] = 1;
//		uint8_t cards[7];
//		cards[0] = hand[0];
//		cards[1] = hand[1];
//		cards[2] = board[0];
//		cards[3] = board[1];
//		cards[4] = board[2];
//		vector<double> bucket(51, 0.0);
//		if (street == 3) {
//			for (int i = 0; i < 52; i++) {
//				if (used[i]) continue;
//				for (int j = i + 1; j < 52; j++) {
//					if (used[j]) continue;
//					int arr[] = { board[0], board[1], board[2], i, j };
//					bucket[(int)(river->data[river->g_index(hand[0], hand[1], arr)] * 50)] += 1;
//				}
//			}
//		}
//		else {
//			// street == 4
//			cards[5] = board[3];
//			for (int i = 0; i < 52; i++) {
//				if (used[i]) continue;
//				cards[6] = i;
//				int arr[] = { board[0], board[1], board[2], board[3], i };
//				bucket[(int)(river->data[river->g_index(hand[0], hand[1], arr)] * 50)] += 1;
//			}
//		}
//		double sum = 0;
//		for (int i = 0; i < 51; i++)
//			sum += bucket[i];
//		for (int i = 0; i < 51; i++)
//			bucket[i] /= sum;
//		return bucket;
//	}
//	vector<double> GetHistogramMC(int street, array<int, 2> hand, array<int, 5> board) {
//		assert(street == 3 || street == 4);
//		bitset<52> used;
//		used[hand[0]] = 1;
//		used[hand[1]] = 1;
//		for (int i = 0; i < street; i++)
//			used[board[i]] = 1;
//		array<int, 7> cards;
//		cards[0] = hand[0];
//		cards[1] = hand[1];
//		cards[2] = board[0];
//		cards[3] = board[1];
//		cards[4] = board[2];
//		auto seed = chrono::high_resolution_clock::now().time_since_epoch().count();
//		mt19937 rng(seed);
//		uniform_int_distribution<int> distribution(0, 51);
//		vector<double> bucket(51, 0.0);
//		if (street == 3) {
//
//			for (int iter = 0; iter < 10000; iter++) {
//				do {
//					cards[5] = distribution(rng);
//				} while (used[cards[5]]);
//				used[cards[5]] = 1;
//				do {
//					cards[6] = distribution(rng);
//				} while (used[cards[6]]);
//				used[cards[6]] = 1;
//				int my_tot = 0, tot = 0;
//				int my_strength = GetHandValue(cards);
//				for (int i = 0; i < 52; i++) {
//					if (used[i]) continue;
//					for (int j = i + 1; j < 52; j++) {
//						if (used[j]) continue;
//						cards[0] = i;
//						cards[1] = j;
//						int opp_strength = GetHandValue(cards);
//						cards[0] = hand[0];
//						cards[1] = hand[1];
//						tot += 2;
//						if (my_strength > opp_strength) my_tot += 2;
//						else if (my_strength == opp_strength) my_tot++;
//					}
//				}
//				bucket[(int)(50.0 * my_tot / tot)] += 1;
//				used[cards[5]] = 0;
//				used[cards[6]] = 0;
//			}
//		}
//		else {
//			// street == 4
//			cards[5] = board[3];
//			for (int i = 0; i < 52; i++) {
//				if (used[i]) continue;
//				cards[6] = i;
//				used[i] = 1;
//				int my_strength = GetHandValue(cards);
//				int my_tot = 0, tot = 0;
//				for (int a = 0; a < 52; a++) {
//					if (used[a]) continue;
//					for (int b = a + 1; b < 52; b++) {
//						if (used[b]) continue;
//						cards[0] = a;
//						cards[1] = b;
//						int opp_strength = GetHandValue(cards);
//						cards[0] = hand[0];
//						cards[1] = hand[1];
//						tot += 2;
//						if (my_strength > opp_strength) my_tot += 2;
//						else if (my_strength == opp_strength) my_tot++;
//					}
//				}
//				bucket[(int)(50.0 * my_tot / tot)] += 1;
//				used[i] = 0;
//			}
//		}
//		double sum = 0;
//		for (int i = 0; i < 51; i++)
//			sum += bucket[i];
//		for (int i = 0; i < 51; i++)
//			bucket[i] /= sum;
//		return bucket;
//	}
//	double EMD(const vector<double>& a, const vector<double>& b) {
//		assert(a.size() == b.size());
//		int n = a.size();
//		double res = 0;
//		double cur = 0;
//		for (int i = 0; i < n; i++) {
//			cur = a[i] + cur - b[i];
//			res += abs(cur);
//		}
//		return res;
//	}
//	vector<double> GetOCHS(array<int, 2> hand, array<int, 5> board) {
//		// only on river
//		bitset<52> used;
//		used[hand[0]] = 1;
//		used[hand[1]] = 1;
//		for (int i = 0; i < 5; i++)
//			used[board[i]] = 1;
//		array<int, 7> schand = { board[0], board[1], board[2], board[3], board[4], hand[0], hand[1] };
//		int my_strength = GetHandValue(schand);
//		vector<double> res;
//		for (int cluster = 0; cluster < 8; cluster++) {
//			int tot = 0;
//			int my_tot = 0;
//			for (array<int, 2> opp_hand : preflop_cluster[cluster]) {
//				if (used[opp_hand[0]] || used[opp_hand[1]])
//					continue;
//				schand[5] = opp_hand[0];
//				schand[6] = opp_hand[1];
//				int opp_strength = GetHandValue(schand);
//				if (my_strength > opp_strength) {
//					my_tot += 2;
//				}
//				else if (my_strength == opp_strength) {
//					my_tot++;
//				}
//				tot += 2;
//			}
//			res.push_back(1.0 * my_tot / tot);
//		}
//		return res;
//	}
//	double GetSquaredEuclideanDist(const vector<double>& a, const vector<double>& b) {
//		assert(a.size() == 8 && b.size() == 8);
//		double res = 0;
//		for (int i = 0; i < 8; i++)
//			res += (a[i] - b[i]) * (a[i] - b[i]);
//		return res;
//	}
//	int GetBucketMC(int street, array<int, 2> hand, array<int, 5> board) {
//		if (street == 0) {
//			int r1 = hand[0] / 4;
//			int r2 = hand[1] / 4;
//			bool suited = ((hand[0] % 4) == (hand[1] % 4));
//			if (r1 > r2) std::swap(r1, r2);
//			if (r1 == r2) {
//				return r1;
//			}
//			int os[12] = { 13, 25, 36, 46, 55, 63, 70, 76, 81, 85, 88, 90 };
//			int ss[12] = { 91, 103, 114, 124, 133, 141, 148, 154, 159, 163, 166, 168 };
//			if (suited) {
//				return ss[r1] + r2 - r1 - 1;
//			}
//			return os[r1] + r2 - r1 - 1;
//		}
//		else if (street == 3) {
//			// create histogram
//			vector<double> hist = GetHistogramMC(street, hand, board);
//			// return closest (emd) center from generated flop centers
//			assert(flop_centers.size());
//			int best_center = 0;
//			double best_dist = EMD(hist, flop_centers[0]);
//			for (int i = 1; i < 100; i++) {
//				double cur_dist = EMD(hist, flop_centers[i]);
//				if (cur_dist < best_dist) {
//					best_dist = cur_dist;
//					best_center = i;
//				}
//			}
//			return best_center;
//		}
//		else if (street == 4) {
//			// create histogram
//			vector<double> hist = GetHistogramMC(street, hand, board);
//			// return closest (emd) center from generated turn centers
//			assert(turn_centers.size());
//			int best_center = 0;
//			double best_dist = EMD(hist, turn_centers[0]);
//			for (int i = 1; i < 100; i++) {
//				double cur_dist = EMD(hist, turn_centers[i]);
//				if (cur_dist < best_dist) {
//					best_dist = cur_dist;
//					best_center = i;
//				}
//			}
//			return best_center;
//		}
//		else {
//			// street == 5
//			// compute equities against 8 opponent hand clusters (1326 lookups)
//			vector<double> OCHS = GetOCHS(hand, board);
//			// return closest (l2) center from generated river centers
//			assert(river_centers.size());
//			int best_center = 0;
//			double best_dist = GetSquaredEuclideanDist(OCHS, river_centers[0]);
//			for (int i = 1; i < (int)river_centers.size(); i++) {
//				double cur_dist = GetSquaredEuclideanDist(OCHS, river_centers[i]);
//				if (cur_dist < best_dist) {
//					best_dist = cur_dist;
//					best_center = i;
//				}
//			}
//			return best_center;
//		}
//	}
//	int GetBucket(int street, array<int, 2> hand, array<int, 5> board) {
//		if (street == 0) {
//			int r1 = hand[0] / 4;
//			int r2 = hand[1] / 4;
//			bool suited = ((hand[0] % 4) == (hand[1] % 4));
//			if (r1 > r2) std::swap(r1, r2);
//			if (r1 == r2) {
//				return r1;
//			}
//			int os[12] = { 13, 25, 36, 46, 55, 63, 70, 76, 81, 85, 88, 90 };
//			int ss[12] = { 91, 103, 114, 124, 133, 141, 148, 154, 159, 163, 166, 168 };
//			if (suited) {
//				return ss[r1] + r2 - r1 - 1;
//			}
//			return os[r1] + r2 - r1 - 1;
//		}
//		else if (street == 3) {
//			// create histogram
//			vector<double> hist = GetHistogram(street, hand, board);
//			// return closest (emd) center from generated flop centers
//			assert(flop_centers.size());
//			int best_center = 0;
//			double best_dist = EMD(hist, flop_centers[0]);
//			for (int i = 1; i < 100; i++) {
//				double cur_dist = EMD(hist, flop_centers[i]);
//				if (cur_dist < best_dist) {
//					best_dist = cur_dist;
//					best_center = i;
//				}
//			}
//			return best_center;
//		}
//		else if (street == 4) {
//			// create histogram
//			vector<double> hist = GetHistogram(street, hand, board);
//			// return closest (emd) center from generated turn centers
//			assert(turn_centers.size());
//			int best_center = 0;
//			double best_dist = EMD(hist, turn_centers[0]);
//			for (int i = 1; i < 100; i++) {
//				double cur_dist = EMD(hist, turn_centers[i]);
//				if (cur_dist < best_dist) {
//					best_dist = cur_dist;
//					best_center = i;
//				}
//			}
//			return best_center;
//		}
//		else {
//			// street == 5
//			// compute equities against 8 opponent hand clusters (1326 lookups)
//			vector<double> OCHS = GetOCHS(hand, board);
//			// return closest (l2) center from generated river centers
//			assert(river_centers.size());
//			int best_center = 0;
//			double best_dist = GetSquaredEuclideanDist(OCHS, river_centers[0]);
//			for (int i = 1; i < (int)river_centers.size(); i++) {
//				double cur_dist = GetSquaredEuclideanDist(OCHS, river_centers[i]);
//				if (cur_dist < best_dist) {
//					best_dist = cur_dist;
//					best_center = i;
//				}
//			}
//			return best_center;
//		}
//	}
//};
//
//int main() {
//	Buckets* bucketer = new Buckets();
//	int arr[] = { 0,1,2,3,4 };
//	//cout << bucketer->river->data[bucketer->river->g_index(1, 5, arr)] << endl;;
//	//cout << bucketer->river->data[bucketer->river->g_index(5, 1, arr)] << endl;;
//	//cout << bucketer->river->data[bucketer->river->g_index(5, 51, arr)] << endl;;
//	//int board[5] = { 2*4+0, 2*4+1, 2*4+2, 2*4+3, 3*4+0 };
//	//int h1 = 12*4+1;
//	//int h2 = 12*4+2;
//	//cout << bucketer->river->data[bucketer->river->g_index(h1, h2, board)] << endl;;
//	int flop_good = 0, flop_bad = 0, turn_good = 0, turn_bad = 0;
//	vector<bool> used(52, false);
//	for (int a = 0; a < 52; a++) {
//		for (int b = a + 1; b < 52; b++) {
//			if (a > 4 && b > 4);
//				//if (bucketer->river->data[bucketer->river->g_index(a, b, arr)] > 0) cout << bucketer->river->data[bucketer->river->g_index(a, b, arr)] << endl;
//		}
//	}
//	//cout << "?" << endl;
//	for (int iter = 0; iter < 1000000; iter++) {
//		if (iter % 1 == 0) {
//			cout << flop_good << ' ' << flop_bad << ' ' << turn_good << ' ' << turn_bad << endl;
//		}
//		mt19937 rng(std::chrono::high_resolution_clock::now().time_since_epoch().count());
//		uniform_int_distribution<int> distribution(0, 51);
//		bitset<52> bs;
//		array<array<int, 2>, 2> hands;
//		array<int, 5> board;
//		for (int a = 0; a < 2; a++) {//
//			for (int b = 0; b < 2; b++) {
//				int x;
//				do {
//					x = distribution(rng);
//				} while (bs[x]);
//				bs[x] = 1;
//				hands[a][b] = x;
//			}
//		}
//		for (int a = 0; a < 5; a++) {
//			int x;
//			do {
//				x = distribution(rng);
//			} while (bs[x]);
//			bs[x] = 1;
//			board[a] = x;
//		}
//		//cout << bucketer->GetBucket(3, hands[0], board) << ' ' << bucketer->GetBucketMC(3, hands[0], board) << endl;
//		//cout << bucketer->GetBucket(4, hands[0], board) << ' ' << bucketer->GetBucketMC(4, hands[0], board) << endl;
//		if (bucketer->GetBucket(3, hands[0], board) == bucketer->GetBucketMC(3, hands[0], board)) {
//			flop_good++;
//		}
//		else {
//			flop_bad++;
//		}
//		if (bucketer->GetBucket(4, hands[0], board) == bucketer->GetBucketMC(4, hands[0], board)) {
//			turn_good++;
//		}
//		else {
//			turn_bad++;
//		}
//	}
//	
//	return 0;
//}
//#include <bits/stdc++.h>
//#include "LUT.h"
//using namespace std;
///***** begin kmeans code *****/
//typedef vector<double> point;
//const int point_dim = 51;
///*double emd(const point& a, const point& b) {
//	assert(a.size() == b.size());
//	double res = 0;
//	for (int d = 0; d < (int)a.size(); d++)
//	{
//		res += (a[d] - b[d]) * (a[d] - b[d]);
//	}
//	return res;
//}*/
//double emd(const vector<double>& a, const vector<double>& b) {
//	assert(a.size() == b.size());
//	int n = a.size();
//	double res = 0;
//	double cur = 0;
//	for (int i = 0; i < n; i++) {
//		cur = a[i] + cur - b[i];
//		res += abs(cur);
//	}
//	return res;
//}
//
//int findclusterid(vector<point>& centers, point pt)
//{
//	int cid = 0;
//	double best_dist = emd(centers[0], pt);
//	for (int i = 1; i < (int)centers.size(); i++)
//	{
//		double cur_dist = emd(centers[i], pt);
//		if (cur_dist < best_dist)
//		{
//			cid = i;
//			best_dist = cur_dist;
//		}
//	}
//	return cid;
//}
//ofstream fout;
//void kmeans(vector<point>& a, int k)
//{
//	mt19937 rng(std::chrono::high_resolution_clock::now().time_since_epoch().count());
//	//init
//	vector<point> centers(k);
//	//kmeans++ initialization
//	uniform_int_distribution<int> int_dist(0, a.size() - 1);
//	centers[0] = a[int_dist(rng)];
//	for (int i = 1; i < k; i++)
//	{
//		cout << i << endl;
//		double sum = 0;
//		vector<double> min_dist(a.size());
//		for (int j = 0; j < (int)a.size(); j++) {
//			min_dist[j] = emd(centers[0], a[j]);
//			for (int k = 1; k < i; k++)
//			{
//				double cur_dist = emd(centers[k], a[j]);
//				if (cur_dist < min_dist[j])
//				{
//					min_dist[j] = cur_dist;
//				}
//			}
//			sum += min_dist[j];
//		}
//		uniform_real_distribution<double> real_dist(0, sum);
//		double sample = real_dist(rng);
//		for (int j = 0; j < (int)a.size(); j++) {
//			sample -= min_dist[j];
//			if (sample > 0) continue;
//			centers[i] = a[j];
//			break;
//		}
//	}
//	vector<int> cluster(a.size(), -1);
//	double lasterr = 0;
//	while (true)
//	{
//		//assigning to cluster
//		for (int i = 0; i < a.size(); i++)
//		{
//			int cid = findclusterid(centers, a[i]);
//			cluster[i] = cid;
//		}
//		//recalculate centers per cluster
//		vector<int> cnt(k, 0);
//		vector<point> sum(k, point(centers[0].size(), 0));
//		double err = 0;
//		for (int i = 0; i < a.size(); i++)
//		{
//			int cid = cluster[i];
//			cnt[cid]++;
//			for (int d = 0; d < (int)a[i].size(); d++)
//			{
//				sum[cid][d] += a[i][d];
//			}
//
//			//error
//			err += emd(a[i], centers[cid]);
//		}
//		double delta = abs(lasterr - err);
//		cout << "error " << err << " delta " << delta << endl;
//		if (delta < 0.1)break;
//		lasterr = err;
//		//assign new centers
//		for (int i = 0; i < k; i++)
//		{
//			for (int d = 0; d < (int)centers[i].size(); d++)
//				centers[i][d] = sum[i][d] / cnt[i];
//		}
//	}
//	//print out results
//	for (int i = 0; i < k; i++)
//	{
//		for (int d = 0; d < point_dim; d++)
//			fout << centers[i][d] << ' ';
//		fout << "\n";
//	}
//}
///***** begin 7-card-eval code *****/
// //cards in rayw implementation are 2c = 1, 2d = 2, ..., as = 52
// //the one and only lookup table.
////int hr[32487834];
//
// //initialize the 2+2 evaluator by loading the handranks.dat file and
// //mapping it to our 32-million member hr array. call this once and
// //forget about it.
///*int inittheevaluator()
//{
//	memset(hr, 0, sizeof(hr));
//	FILE* fin = fopen("handranks.dat", "rb");
//	// load the handranks.dat file data into the hr array
//	size_t bytesread = fread(hr, sizeof(hr), 1, fin);
//	fclose(fin);
//}*/
//
// //given a group of 7 cards, return the hand category & rank. let
// //pcards be (a pointer to) an array of seven integers, each with
// //a value between 1 and 52.
//
//
// /*set up a 7-card poker hand, perform the lookup into the table, and
// extract the category (full house, two pair, flush, etc.) and rank
// (within that category) of the hand. these numbers can be used to
// compare the hand to other hands.*/
///*void dosomework()
//{
//	int mycards[] = { 1, 5, 9, 13, 51, 49 };
//	int handinfo = gethandvalue(mycards);
//	int handcategory = handinfo >> 12;
//	int rankwithincategory = handinfo & 0x00000fff;
//}*/
//
//int convert(string s) {
//	char rank = s[0];
//	char suit = s[1];
//	string ranks = "23456789tjqka";
//	string suits = "shcd";
//	int r, ss;
//	for (int i = 0; i < 13; i++)
//		if (rank == ranks[i])
//			r = i;
//	for (int i = 0; i < 4; i++)
//		if (suit == suits[i])
//			ss = i;
//	return 4 * r + ss + 1;
//}
//
//string deconvert(int n) {
//	int r = n / 4;
//	int s = n % 4;
//	string ranks = "23456789tjqka";
//	string suits = "shcd";
//	string res = "";
//	res += ranks[r] + suits[s];
//	return res;
//}
//
// //ako or aks or aa to a vector of card indices (0-indexed)
//vector<pair<int, int> > convertt(string s) {
//	string ranks = "23456789tjqka";
//	string suits = "shcd";
//	map<char, int> rank_rev, suit_rev;
//	int i = 0;
//	for (char c : ranks) rank_rev[c] = i++;
//	i = 0;
//	for (char c : suits) suit_rev[c] = i++;
//	vector<pair<int, int> > res;
//	if (s.size() == 2) {
//		assert(s[0] == s[1]);
//		 //pocket pairs
//		int f = 4 * rank_rev[s[0]];
//		for (int i = 0; i < 4; i++)
//			for (int j = i + 1; j < 4; j++)
//				res.push_back(make_pair(f + i, f + j));
//		return res;
//	}
//	assert(s.size() == 3);
//	int r1 = 4 * rank_rev[s[0]];
//	int r2 = 4 * rank_rev[s[1]];
//	if (s.back() == 's') {
//		 //suited
//		for (int i = 0; i < 4; i++)
//			res.push_back(make_pair(r1 + i, r2 + i));
//	}
//	else {
//		assert(s.back() == 'o');
//		 //unsuited
//		for (int i = 0; i < 4; i++) {
//			for (int j = 0; j < 4; j++) {
//				if (i == j) continue;
//				res.push_back(make_pair(r1 + i, r2 + j));
//			}
//		}
//	}
//	return res;
//}
//
//string pf_cluster[8] = { "23s,24s,25s,26s,27s,34s,35s,36s,37s,45s,46s,32o,43o,42o,54o,53o,52o,65o,64o,63o,62o,74o,73o,72o,83o,82o",
//"28s,29s,2ts,38s,39s,47s,48s,49s,75o,85o,84o,95o,94o,93o,92o,t5o,t4o,t3o,t2o,j3o,j2o",
//"3ts,4ts,56s,57s,58s,59s,5ts,67s,68s,69s,6ts,78s,79s,89s,67o,68o,69o,6to,78o,79o,7to,89o,8to",
//"22,j2s,j3s,j4s,j5s,j6s,q2s,q3s,q4s,q5s,k2s,j4o,j5o,j6o,j7o,q2o,q3o,q4o,q5o,q6o,q7o,k2o,k3o,k4o",
//"6qs,7ts,7js,7qs,8ts,8js,8qs,9ts,9js,9qs,tjs,t9o,j8o,j9o,jto,q8o,q9o,qto,qjo",
//"33,44,55,k3s,k4s,k5s,k6s,k7s,k8s,a2s,a3s,a4s,a5s,a6s,k5o,k6o,k7o,k8o,k9o,a2o,a3o,a4o,a5o,a6o,a7o,a8o",
//"66,77,qts,qjs,k9s,kts,kjs,kqs,a7s,a8s,a9s,ats,ajs,aqs,aks,kto,kjo,kqo,a9o,ato,ajo,aqo,ako",
//"88,99,tt,jj,qq,kk,aa" };
//vector<pair<int, int> > preflop_cluster[8];
//
//void split_clusters() {
//	for (int i = 0; i < 8; i++) {
//		vector<string> split_cluster;
//		istringstream ss(pf_cluster[i]);
//		string token;
//		while (getline(ss, token, ',')) {
//			split_cluster.push_back(token);
//			//cout << token << endl;
//		}
//		for (string s : split_cluster) {
//			vector<pair<int, int> > tmp = convertt(s);
//			for (pair<int, int> x : tmp) preflop_cluster[i].push_back(x);
//		}
//	}
//}
//Poker_Abstraction* hsdgg;
//int GetHandValue(array<int, 7> cards) {
//	int p = hsdgg->hr[53 + cards[0] + 1];
//	p = hsdgg->hr[p + cards[1] + 1];
//	p = hsdgg->hr[p + cards[2] + 1];
//	p = hsdgg->hr[p + cards[3] + 1];
//	p = hsdgg->hr[p + cards[4] + 1];
//	p = hsdgg->hr[p + cards[5] + 1];
//	return hsdgg->hr[p + cards[6] + 1];
//}
//int gethandvalue(int* pcards)
//{
//	int p = hsdgg->hr[53 + *pcards++];
//	p = hsdgg->hr[p + *pcards++];
//	p = hsdgg->hr[p + *pcards++];
//	p = hsdgg->hr[p + *pcards++];
//	p = hsdgg->hr[p + *pcards++];
//	p = hsdgg->hr[p + *pcards++];
//	return hsdgg->hr[p + *pcards++];
//}
//CLutRiver* river;
//vector<double> GetHistogram(int street, array<int, 2> hand, array<int, 5> board) {
//	assert(street == 3 || street == 4);
//	bitset<52> used;
//	used[hand[0]] = 1;
//	used[hand[1]] = 1;
//	for (int i = 0; i < street; i++)
//		used[board[i]] = 1;
//	vector<double> bucket(51, 0.0);
//	if (street == 3) {
//		for (int i = 0; i < 52; i++) {
//			if (used[i]) continue;
//			for (int j = i + 1; j < 52; j++) {
//				if (used[j]) continue;
//				int arr[] = { board[0], board[1], board[2], i, j };
//				bucket[(int)(river->data[river->g_index(hand[0], hand[1], arr)] * 50)] += 1;
//			}
//		}
//	}
//	else {
//		// street == 4
//		for (int i = 0; i < 52; i++) {
//			if (used[i]) continue;
//			int arr[] = { board[0], board[1], board[2], board[3], i };
//			bucket[(int)(river->data[river->g_index(hand[0], hand[1], arr)] * 50)] += 1;
//		}
//	}
//	double sum = 0;
//	for (int i = 0; i < 51; i++)
//		sum += bucket[i];
//	for (int i = 0; i < 51; i++)
//		bucket[i] /= sum;
//	return bucket;
//}
//
//int isomorphize(int h1, int h2) {
//	int r1 = h1 / 4;
//	int r2 = h2 / 4;
//	int s1 = h1 % 4;
//	int s2 = h2 % 4;
//	if (r1 > r2) swap(r1, r2);
//	int suited = 0;
//	if (s1 == s2) suited = 1;
//	return (r1 * 100 + r2) * 10 + suited;
//}
//
//int main() {
//	cout << "start" << endl;
//	fout.open("newturncenters.txt");
//	fout << fixed << setprecision(8);
//	hsdgg = new Poker_Abstraction();
//	//CLutFlop* flop = new CLutFlop();
//	river = new CLutRiver();
//	river->read_suitfile("river_suit.dat");
//	river->load("river_lut.dat");
//	//turn->read_suitfile("turn_suit.dat");
//	//turn->load("turn_lut.dat");
//	vector<vector<double> > values;
//	for (int iter = 0; iter < 500000; iter++) {
//		if (iter % 10000 == 0) {
//			cout << iter << endl;
//		}
//		bitset<53> bs;
//		auto seed = chrono::high_resolution_clock::now().time_since_epoch().count();
//		mt19937 rng(seed);
//		uniform_int_distribution<int> distribution(0, 51);
//		int a[7];
//		for (int i = 0; i < 7; i++) {
//			int x;
//			do {
//				x = distribution(rng);
//			} while (bs[x]);
//			a[i] = x;
//			bs[x] = 1;
//		}
//
//		array<int, 2> my_hand = { a[0], a[1] };
//		array<int, 5> board = { a[2],a[3],a[4],a[5],a[6] };
//		
//		values.push_back(GetHistogram(4, my_hand, board));
//		bs.reset();
//	}
//	//split_clusters();
//	 //generate river ochs
//	/*vector<vector<double> > values;
//	for (int iter = 0; iter < 500000; iter++) {
//		if (iter % 10000 == 0) {
//			cout << iter << endl;
//		}
//		bitset<53> bs;
//		auto seed = chrono::high_resolution_clock::now().time_since_epoch().count();
//		mt19937 rng(seed);
//		uniform_int_distribution<int> distribution(1, 52);
//		int a[7];
//		for (int i = 0; i < 7; i++) {
//			int x;
//			do {
//				x = distribution(rng);
//			} while (bs[x]);
//			a[i] = x;
//			bs[x] = 1;
//		}
//		int my_value = gethandvalue(a);
//		vector<double> ochs(8);
//		for (int i = 0; i < 8; i++) {
//			int tot = 0, my_tot = 0;
//			for (pair<int, int> p : preflop_cluster[i]) {
//				if (bs[p.first + 1] || bs[p.second + 1]) continue;
//				a[5] = p.first + 1;
//				a[6] = p.second + 1;
//				int opp_value = gethandvalue(a);
//				if (my_value > opp_value) my_tot += 2;
//				else if (my_value == opp_value) my_tot += 1;
//				tot += 2;
//			}
//			ochs[i] = 1.0 * my_tot / tot;
//		}
//		values.push_back(ochs);
//		bs.reset();
//	}
//	/*vector<point> a;
//	for (int i = 0; i < 200000; i++) {
//		point p(point_dim);
//		for (int j = 0; j < point_dim; j++)
//			fin >> p[j];
//		a.push_back(p);
//	}
//	cout << "starting" << endl;*/
//	int k = 100;
//	kmeans(values, k);
//	return 0;
//}

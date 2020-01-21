#pragma once
#include <vector>
#include <string>
using namespace std;

const int STARTING_STACK = 200;
const int BIG_BLIND = 2;
const int SMALL_BLIND = 1;

class GameTreeNode {
public:
	GameTreeNode();
	vector<GameTreeNode*> children;
	virtual double TrainExternalSampling(int trainplayer, int* hands, double p, double op);
	virtual double BestResponse(int brplayer, int hand, const vector<double>& op);
	virtual void PrintStrategy(int* hands, int depth, string history);
	~GameTreeNode();
};
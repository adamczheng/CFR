#include "GameTreeNode.h"
class Showdown : public GameTreeNode {
public:
	double value;
	Showdown(double val) {
		value = val;
	}
	virtual double TrainExternalSampling(int trainplayer, int* hands, double p, double op) override {
		return hands[trainplayer] > hands[trainplayer ^ 1] ? value : -value;
	}
	virtual double BestResponse(int brplayer, int hand, const vector<double>& op) override {
		double ev = 0;
		for (int i = 0; i < (int)op.size(); i++) {
			if (hand > i)
				ev += value * op[i];
			else if (hand < i)
				ev -= value * op[i];
		}
		return ev;
	}
};
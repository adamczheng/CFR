#include "GameTreeNode.h"
class Fold : public GameTreeNode {
public:
	double value;
	Fold(double val) {
		value = val;
	}
	virtual double TrainExternalSampling(int trainplayer, int* hands, double p, double op) override {
		// why ?
		if (trainplayer == 0)
			return value;
		else
			return -value;
	}
	virtual double BestResponse(int brplayer, int hand, const vector<double>& op) override {
		double ev = 0;
		for (int i = 0; i < (int)op.size(); i++)
			ev += value * op[i];
		return brplayer == 0 ? ev : -ev;
	}
};
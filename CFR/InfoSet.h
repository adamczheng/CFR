#include <vector>
using std::vector;
class InfoSet {
public:
	// index i refers to child i
	vector<double> regret;
	vector<double> cumulativeStrategy;
};
InfoSet iset[6][200];
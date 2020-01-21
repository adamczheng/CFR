#include "Deck.h"
class Iteration {
private:
	static const int PLAYER_COUNT = 2;
	Deck deck;
	int holes[PLAYER_COUNT];
	int flop, turn;
	int buckets[4][PLAYER_COUNT];
	int handRanks[PLAYER_COUNT];
public:
	Iteration()
	{
		for (int i = 0; i < PLAYER_COUNT; i++)
			holes[i] = deck.GetCard();

		flop = deck.GetCard();
		turn = deck.GetCard();

		for (int i = 0; i < PLAYER_COUNT; i++)
		{
			//buckets[0][i] = Buckets.GetBucket(0, holes[i]);
			//buckets[1][i] = Buckets.GetBucket(1, holes[i], flop);
			//buckets[2][i] = Buckets.GetBucket(2, holes[i], flop, turn);
			//buckets[3][i] = Buckets.GetBucket(3, holes[i], flop, turn, river);
			//handRanks[i] = HandRank.GetHandRank(holes[i], flop, turn);
		}
	}

	int GetBucket(int street, int player)
	{
		return buckets[street][player];
	}

	double GetShowdownValue(int player)
	{
		if (handRanks[player] > handRanks[player ^ 1])
			return 1.0;
		else if (handRanks[player] < handRanks[player ^ 1])
			return -1.0;
		else
			return 0.0;
	}
};
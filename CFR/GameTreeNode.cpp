#include "GameTreeNode.h"
#include <iostream>
#include <string>

GameTreeNode::GameTreeNode()
{
}

double GameTreeNode::TrainExternalSampling(int trainplayer, int* hands, double p, double op)
{
	return 600;
}

double GameTreeNode::BestResponse(int brplayer, int hand, const vector<double>& op)
{
	return 600;
}

void GameTreeNode::PrintStrategy(int* hands, int depth, string history)
{
	//for (int i = 0; i < depth; i++) std::cout << ' ';
	//std::cout<<history<<std::endl;
}

GameTreeNode::~GameTreeNode()
{
}
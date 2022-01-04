#pragma once
#include "projects/MachineLearning/QBot.h"

class Population
{
public:
	Population(int size, int nrOfFood, int nrOfInputs, int nrOfOutputs, bool bias);
	~Population();

	void Update(float deltaTime);
	void Render(float deltaTime) const;

private:
	int m_Size;
	int m_NrOfFood;
	//bool m_AllBotsDead;
	int m_DeadCounter;
	int m_Generation;
	std::vector<QBot*> m_Bots;
	std::vector<vector<Food*>> m_Foodstuff;
};


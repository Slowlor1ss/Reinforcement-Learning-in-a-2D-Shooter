/*=============================================================================*/
// Copyright 2021-2022 Elite Engine
// Authors: Koen Samyn
/*=============================================================================*/
#ifndef DYNAMIC_Q_LEARNING
#define DYNAMIC_Q_LEARNING

//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------

#include "Population.h"
#include "QBot.h"
class DynamicQLearning final
{
public:
	DynamicQLearning(int nrOfFood, int memorySize, int nrOfInputs, int nrOfOutputs, bool bias );
	~DynamicQLearning();

	void Update(float deltaTime) const;
	void Render(float deltaTime) const;

	//QBot* GetQBot() const { return m_pQBot; }
	//QBot* GetQBot2() const { return m_pQBot2; }

private:
	static constexpr int MEMORY_SIZE = 200;

	Population* m_pPopulation;

	//int m_MemorySize;
	//int m_NrOfInputs;
	//int m_NrOfOutputs;
	//bool m_UseBias;

	// currentIndex stores the information at the current time.
	// instead of swapping or copying matrices, manipulate the currentIndex to
	// go back in time.
	//int currentIndex = 0;
	//QBot* m_pQBot{ nullptr };
	//QBot* m_pQBot2{ nullptr };

	// environment
	//vector<Food*> m_Foodstuff;
};

#endif

/*=============================================================================*/
// Copyright 2021-2022 Elite Engine
// Authors: Koen Samyn
/*=============================================================================*/
#ifndef DYNAMIC_Q_LEARNING
#define DYNAMIC_Q_LEARNING

//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------

#include "QBot.h"
class DynamicQLearning final
{
public:
	DynamicQLearning(int nrOfFood, int memorySize, int nrOfInputs, int nrOfOutputs, bool bias );
	~DynamicQLearning();

	void Update(float deltaTime);
	void Render(float deltaTime);

	QBot* GetQBot() const { return m_pQBot; }

private:
	static const int MEMORY_SIZE = 200;
	
	int m_MemorySize;
	int m_NrOfInputs;
	int m_NrOfOutputs;
	bool m_UseBias;

	// currentIndex stores the information at the current time.
	// instead of swapping or copying matrices, manipulate the currentIndex to
	// go back in time.
	int currentIndex = 0;
	QBot* m_pQBot{ 0 };

	// environment
	vector<Food*> m_Foodstuff;
};

#endif

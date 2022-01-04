//=== General Includes ===
#include "stdafx.h"
#include "DynamicQLearning.h"
#include "Food.h"


DynamicQLearning::DynamicQLearning(int nrOfFood, int memorySize, int nrOfInputs, int nrOfOutputs, bool bias)
	//:m_MemorySize(memorySize),
	//m_NrOfInputs(nrOfInputs),
	//m_NrOfOutputs(nrOfOutputs),
	//m_UseBias(bias)
	: m_pPopulation(new Population(50, nrOfFood, nrOfInputs, nrOfOutputs, bias))
{
	//float startx = Elite::randomFloat(-50.0f, 50.0f);
	//float starty = Elite::randomFloat(-50.0f, 50.0f);
	////float startx2 = Elite::randomFloat(-50.0f, 50.0f);
	////float starty2 = Elite::randomFloat(-50.0f, 50.0f);
	//float startAngle = Elite::randomFloat(0, static_cast<float>(M_PI) * 2);
	//
	//// memory is 100.
	//m_pQBot = new QBot(startx, starty, float(M_PI) / 2 /*3*/, /*2*/ 3 * float(M_PI), startAngle, 50, m_NrOfInputs, m_NrOfOutputs, m_UseBias, 2.f, "bot1");
	////m_pQBot2 = new QBot(startx2, starty2, float(M_PI) / 2 /*3*/, /*2*/ 3 * float(M_PI), startAngle, 50, m_NrOfInputs, m_NrOfOutputs, m_UseBias, 2.f, "bot2");

	////Initialization of your application. If you want access to the physics world you will need to store it yourself.
	//for (int i = 0; i < nrOfFood; ++i)
	//{
	//	//float angle = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX));
	//	//angle *= 2 * float(M_PI);
	//	//float dist = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX));
	//	//dist *= 65;
	//	//dist += 20;
	//	//TODO: change 70 to m_TrimWorldSize
	//	Food* f = new Food(randomFloat(-67, 67), randomFloat(-67, 67));
	//	//Food* f = new Food(dist * cos(angle), dist * sin(angle));
	//	m_Foodstuff.push_back(f);
	//}

}

DynamicQLearning::~DynamicQLearning() 
{
	SAFE_DELETE(m_pPopulation);
	//SAFE_DELETE(m_pQBot);
	////SAFE_DELETE(m_pQBot2);
	//for (Food* &pFood : m_Foodstuff) {
	//	SAFE_DELETE(pFood);
	//}
}
void DynamicQLearning::Update(const float deltaTime) const
{
	m_pPopulation->Update(deltaTime);
	//m_pQBot->Update(m_Foodstuff, deltaTime);
	////m_pQBot2->Update(m_Foodstuff, deltaTime);
	//for (Food* pFood : m_Foodstuff) {
	//	pFood->Update();
	//}
}

void DynamicQLearning::Render(const float deltaTime) const
{
	m_pPopulation->Render(deltaTime);
	//for (Food* pFood : m_Foodstuff) {
	//	pFood->Render();
	//}
	//m_pQBot->Render(deltaTime);
}

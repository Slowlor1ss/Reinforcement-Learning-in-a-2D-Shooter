#include "stdafx.h"
#include "Population.h"

#include <iomanip>

Population::Population(int size, int nrOfFood, int nrOfInputs, int nrOfOutputs, bool bias)
	: m_Size(size)
	, m_NrOfFood(nrOfFood)
	//, m_AllBotsDead(false)
	, m_DeadCounter(0)
	, m_Generation(0)
{
	const float startx = Elite::randomFloat(-50.0f, 50.0f);
	const float starty = Elite::randomFloat(-50.0f, 50.0f);
	vector<Food*> foodstuff;
	for (int i = 0; i < nrOfFood; ++i)
	{
		const float foodx = randomFloat(-67, 67);
		const float foody = randomFloat(-67, 67);

		Food* f = new Food(foodx, foody);
		foodstuff.push_back(f);
	}

	for (size_t i{0}; i < size; ++i)
	{
		const float startAngle = Elite::randomFloat(0, static_cast<float>(M_PI) * 2);
		// memory is 100.
		m_Bots.push_back(new QBot(startx, starty, static_cast<float>(M_PI) / 2 /*3*/, /*2*/ 3 * static_cast<float>(M_PI), startAngle, 50, nrOfInputs, nrOfOutputs, bias, 2.f));

		m_Foodstuff.push_back(foodstuff);
	}
}

Population::~Population()
{
	for (auto bot : m_Bots)
	{
		SAFE_DELETE(bot);
	}

	for (std::vector<Food*>& vFood : m_Foodstuff) {
		for (Food*& pFood : vFood) {
			SAFE_DELETE(pFood);
		}
	}
}

void Population::Update(const float deltaTime)
{
	// Check if all bods are dead
	if (m_DeadCounter == m_Size)
	{
		//m_AllBotsDead = true;

		++m_Generation;

		QBot* BestBot = *std::max_element(m_Bots.begin(), m_Bots.end());
		cout << "Died after " << std::setprecision(4) << BestBot->GetAge() << " seconds.\nGeneration: " << m_Generation << endl;

		const auto BestBrain = BestBot->GetBotBrain();

		if (m_Generation%10 == 0)
		{
			(*BestBrain).Print();
		}

		for (size_t i{ 0 }; i < m_Bots.size(); ++i)
		{
			m_Bots[i]->Reset();
			if(m_Bots[i] == BestBot)
				continue;
			m_Bots[i]->SetBotBrain(*BestBrain);
			//TODO: natural selection
		}

		m_DeadCounter = 0;
		return;
	}

	m_DeadCounter = 0;
	for (size_t i{0}; i < m_Bots.size(); ++i)
	{
		// Check if bot is not dead
		if (m_Bots[i]->IsAlive())
		{
			// Update bots
			m_Bots[i]->Update(m_Foodstuff[i], deltaTime);

			// Update food
			for (const auto pFood : m_Foodstuff[i])
				pFood->Update();
		}
		else
			++m_DeadCounter;
	}

	// Clean up dead bots
	//for (size_t i{0}; i < m_DeadBots.size(); ++i)
	//{
		//SAFE_DELETE(m_Bots[m_DeadBots[i]]);
		//for (size_t j{0}; j < m_NrOfFood; ++j)
		//{
		//	SAFE_DELETE(m_Foodstuff[m_DeadBots[i]][j]);
		//}
	//}

	//for (const std::vector<Food*>& vFood : m_Foodstuff) {
	//	for (const auto pFood : vFood)
	//	{
	//		pFood->Update();
	//	}
	//}
}

void Population::Render(const float deltaTime) const
{
	for (size_t i{ 0 }; i < m_Bots.size(); ++i)
	{
		if (m_Bots[i]->IsAlive())
		{
			m_Bots[i]->Render(deltaTime);
			for (const auto pFood : m_Foodstuff[i])
				pFood->Render();
		}
	}
}

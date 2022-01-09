#include "stdafx.h"
#include "Population.h"
#include "projects/MachineLearning/QBot.h"
#include "Food.h"
#include "projects/Shared/NavigationColliderElement.h"
#include <iomanip>
#include <numeric>
#include <execution>

Population::Population(int size, int nrOfFood, int nrOfInputs, int nrOfOutputs, bool bias)
	: m_Size(size)
	, m_NrOfFood(nrOfFood)
	//, m_AllBotsDead(false)
	, m_DeadCounter(0)
	, m_Generation(0)
{
	const float startx = Elite::randomFloat(-50.0f, 50.0f);
	const float starty = Elite::randomFloat(-50.0f, 50.0f);
	//TODO:create bounderys in app machine learning and pass it to here
	//Create Boundaries
	constexpr float blockSize{ 5.0f };
	constexpr float hBlockSize{ blockSize / 2.0f };
	m_vNavigationColliders.push_back(new NavigationColliderElement(Elite::Vector2(-70 - hBlockSize, 0.f), blockSize, (70 + blockSize) * 2.0f));
	m_vNavigationColliders.push_back(new NavigationColliderElement(Elite::Vector2(70 + hBlockSize, 0.f), blockSize, (70 + blockSize) * 2.0f));
	m_vNavigationColliders.push_back(new NavigationColliderElement(Elite::Vector2(0.0f, 70 + hBlockSize), 70 * 2.0f, blockSize));
	m_vNavigationColliders.push_back(new NavigationColliderElement(Elite::Vector2(0.0f, -70 - hBlockSize), 70 * 2.0f, blockSize));

	m_vNavigationColliders.push_back(new NavigationColliderElement(Elite::Vector2(-70.0f + (36 / 2.f), -50.0f + (6 / 2.f)), 36, 6));
	m_vNavigationColliders.push_back(new NavigationColliderElement(Elite::Vector2(-70.0f + (76 / 2.f), -28.0f + (6 / 2.f)), 76, 6));
	m_vNavigationColliders.push_back(new NavigationColliderElement(Elite::Vector2(18.0f + (36 / 2.f), -28.0f + (6 / 2.f)), 36, 6));
	m_vNavigationColliders.push_back(new NavigationColliderElement(Elite::Vector2(-70.0f + (36 / 2.f), -6.0f + (6 / 2.f)), 36, 6));
	m_vNavigationColliders.push_back(new NavigationColliderElement(Elite::Vector2(-35.0f + (36 / 2.f), 15.0f + (6 / 2.f)), 36, 6));
	m_vNavigationColliders.push_back(new NavigationColliderElement(Elite::Vector2(34.0f + (36 / 2.f), 15.0f + (6 / 2.f)), 36, 6));
	m_vNavigationColliders.push_back(new NavigationColliderElement(Elite::Vector2(-70.0f + (84 / 2.f), 43.0f + (6 / 2.f)), 84, 6));
	m_vNavigationColliders.push_back(new NavigationColliderElement(Elite::Vector2(34.0f + (36 / 2.f), 43.0f + (6 / 2.f)), 36, 6));

	std::vector<float> foodx{};
	foodx.reserve(nrOfFood);
	std::vector<float> foody{};
	foody.reserve(nrOfFood);
	for (int j = 0; j < nrOfFood; ++j)
	{
		foodx.push_back(randomFloat(-67, 67));
		foody.push_back(randomFloat(-67, 67));
	}

	m_Foodstuff.reserve(size);
	for (size_t i{0}; i < size; ++i)
	{
		const float startAngle = Elite::randomFloat(0, static_cast<float>(M_PI) * 2);
		// memory is 100.
		m_Bots.push_back(new QBot(startx, starty, static_cast<float>(M_PI) / 2 /*3*/, 2 /*3*/ * static_cast<float>(M_PI), startAngle, 50, nrOfInputs, nrOfOutputs, bias, 2.f));
		m_Bots.back()->SetObstacles(m_vNavigationColliders);

		m_Foodstuff.emplace_back();
		m_Foodstuff[i].reserve(nrOfFood);
		for (int j = 0; j < nrOfFood; ++j)
		{
			m_Foodstuff[i].push_back(new Food(foodx[j], foody[j]));
		}
	}
}

Population::~Population()
{
	for (auto bot : m_Bots)
	{
		SAFE_DELETE(bot);
	}

	for (std::vector<Food*> vFood : m_Foodstuff) {
		for (Food* pFood : vFood) {
			SAFE_DELETE(pFood);
		}
	}

	for (auto pNC : m_vNavigationColliders)
		SAFE_DELETE(pNC);
	m_vNavigationColliders.clear();
}

void Population::Update(const float deltaTime)
{
	// Check if all bods are dead
	if (m_DeadCounter == m_Size)
	{
		//m_AllBotsDead = true;

		++m_Generation;

		//Stochastic universal sampling

		//TODO: implement some debug toggle and toggle this
		QBot* BestBot = *std::max_element(m_Bots.begin(), m_Bots.end());
		cout << "Died after " << std::setprecision(4) << BestBot->GetAge() << " seconds.\nGeneration: " << m_Generation << endl;

		if (m_Generation%10 == 0)
		{
			const auto BestBrain = BestBot->GetBotBrain();
			(*BestBrain).Print();
		}

		SelectParentSUS();
		for (size_t i{ 0 }; i < m_Bots.size(); ++i)
		{
			//if(m_Bots[i] != BestBot)
				//m_Bots[i]->SetBotBrain(*BestBrain);
				//m_Bots[i]->SetBotBrain(*SelectParent(sum));
			m_Bots[i]->UniformCrossover(m_Bots[randomInt(static_cast<int>(m_Bots.size()))]);
			m_Bots[i]->MutateMatrix(0.1f, 0.3f);
			m_Bots[i]->Reset();
		}

		//Fitness proportionate selection

		//QBot* BestBot = *std::max_element(m_Bots.begin(), m_Bots.end());
		//cout << "Died after " << std::setprecision(4) << BestBot->GetAge() << " seconds.\nGeneration: " << m_Generation << endl;
		//
		//if (m_Generation % 10 == 0)
		//{
		//	const auto BestBrain = BestBot->GetBotBrain();
		//	(*BestBrain).Print();
		//}
		//
		//const float sum{ CalculateFitnessSum() };
		//
		//for (size_t i{ 0 }; i < m_Bots.size(); ++i)
		//{
		//	m_Bots[i]->SetBotBrain(*SelectParentFPS(sum));
		//	m_Bots[i]->Reset();
		//}

		//Tournament selection

		//QBot* BestBot = *std::max_element(m_Bots.begin(), m_Bots.end());
		//cout << "Died after " << std::setprecision(4) << BestBot->GetAge() << " seconds.\nGeneration: " << m_Generation << endl;
		//const auto BestBrain = BestBot->GetBotBrain();
		//
		//if (m_Generation % 10 == 0)
		//{
		//	(*BestBrain).Print();
		//}
		//
		//for (size_t i{ 0 }; i < m_Bots.size(); ++i)
		//{
		//	if(m_Bots[i] != BestBot)
		//		m_Bots[i]->SetBotBrain(*BestBrain);
		//
		//	m_Bots[i]->Reset();
		//}

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
			for (const auto pFood : m_Foodstuff[i]) //TODO: only render one iteration of food as they have the same location maybe
				pFood->Render();
		}
	}
	for (const auto collider : m_vNavigationColliders)
	{
		collider->RenderElement();
	}
}

//Struct for calculating the sum i couldn't overload pointers so i had to create a function
struct Calculator
{
	float operator() (const float left, const QBot* other) const
	{
		return left + other->GetFitness();
	}
	float operator() (const QBot* other, const float left) const
	{
		return left + other->GetFitness();
	}
	float operator() (const QBot* bot, const QBot* other) const
	{
		return bot->GetFitness() + other->GetFitness();
	}
	float operator() (const float left, const float right) const
	{
		return left + right;
	}
};

//https://en.cppreference.com/w/cpp/algorithm/reduce
float Population::CalculateFitnessSum() const
{
	return std::reduce(std::execution::par, m_Bots.cbegin(), m_Bots.cend(), 0.f, Calculator());
	//return std::accumulate(m_Bots.cbegin(), m_Bots.cend(), 0.f, Sum);
}
float Population::CalculateFitnessSum(unsigned int from, unsigned int to) const
{
	if (to == 0)
		return m_Bots[0]->GetFitness();
	if (to <= from){
		std::cout << "to was smaller then or equal to from, putting it to from+1\n";
		to = from + 1;
	}
	if (to > m_Bots.size()-1){
		std::cout << "to was too big, putting it to max possible value\n";
		to = m_Bots.size() - 1;
	}

	return std::reduce(std::execution::par, m_Bots.cbegin()+int(from), m_Bots.cbegin()+int(to), 0.f, Calculator());
}

//Natural Selection
//Code that selects a parent to inherent the brain from,
//agents with higher fitness will have more chance to pass on their brain than agents with lower fitness
//Fitness proportionate selection
Elite::FMatrix* Population::SelectParentFPS(const float sum) const
{
	const float rand{ randomFloat(sum) };

	float runningSum{};

	for (size_t i{0}; i < m_Bots.size(); ++i)
	{
		runningSum += m_Bots[i]->GetFitness();

		if (runningSum >= rand)
			return m_Bots[i]->GetBotBrain();
	}

	//Code should never reach this far if; it does the calculate sum function is wrong
	std::cout << __LINE__ << __FILE__ << "\nCode reached something it shouldnt've";
	return nullptr;
}

//Stochastic universal sampling
void Population::SelectParentSUS() const
{
	std::vector<float> pointers{};
	auto num{ m_Bots.size() }; //Number of offspring to keep
	auto dist = num / static_cast<float>(num);
	auto start = randomFloat(0, dist);
	for (size_t i{0}; i < num; ++i)
	{
		pointers.push_back(start + static_cast<float>(i) * dist);
	}

	for (size_t i{0}; i < pointers.size(); ++i)
	{
		auto j = 0;
		while (CalculateFitnessSum(0, j) < pointers[i])
		{
			j++;
		}
		m_Bots[i]->SetBotBrain(*m_Bots[j]->GetBotBrain());
	}
}

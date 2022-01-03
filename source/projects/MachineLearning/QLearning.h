#pragma once
#include "framework/EliteMath/EVector2.h"
#include <chrono>

class QLearning final
{
public:
	QLearning(int nrOfLocations, int startIndex, int endIndex);
	~QLearning();

	void SetLocation(int index, Elite::Vector2 location);
	void AddConnection(int from, int to);
	void AddTreasureLocation(int loc);
	void AddKoboldLocation(int loc);

	void Train();
	void TrainEnvironment();
	void TrainWithEnvironment();
	void Render(float deltaTime);

	void PrintRewardMatrix();
	void PrintQMatrix();

protected:
	int SelectAction(int currentLoc);
	float Update(int currentLoc, int nextAction);
	int SelectActionWithEnv(int currentLoc);
private:
	int m_NrOfLocations;
	int m_StartIndex;
	int m_EndIndex;
	float m_Gamma{ 0.8f };
	int m_NrOfIterations{ 2000 };
	int m_CurrentIteration{ 0 };
	std::vector<Elite::Vector2> m_Locations;
	Elite::FMatrix* m_pRewardMatrix{ 0 };
	Elite::FMatrix* m_pQMatrix{ 0 };
	Elite::FMatrix* m_pTreasureMatrix{ 0 };
	Elite::FMatrix* m_pKoboldMatrix{ 0 };
	Elite::FMatrix* m_pEnvMatrix{ 0 };

	std::vector<int> m_KoboldLocations;
	std::vector<int> m_TreasureLocations;
	int* m_pIndexBuffer{ 0 };
	//std::vector<int> m_pIndexBuffer{};

	// colors
	//TODO: (ANDRIES) Change to common color pallet
	Elite::Color m_NormalColor{ 0.0f, 1.0f, 1.0f, 1.0f };
	Elite::Color m_StartColor{ 0.0f, 1.0f, 0.0f, 1.0f };
	Elite::Color m_EndColor{ 1.0f, 0.0f, 0.0f, 1.0f };
	Elite::Color m_ConnectionColor{ 1.0f, 1.0f, 0.0f, 1.0f };
	Elite::Color m_NoQConnection{ 0.5f, 0.0f, 0.0f, 1.0f };
	Elite::Color m_MaxQConnection{ 0.1f, 1.0f, 1.0f, 1.0f };

	chrono::steady_clock::time_point start{};
	chrono::steady_clock::time_point stop{};

	//C++ make the class non-copyable
	QLearning(const QLearning&) = delete;
	QLearning& operator=(const QLearning&) = delete;
};


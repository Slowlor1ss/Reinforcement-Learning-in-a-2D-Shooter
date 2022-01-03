#ifndef QBOT_H
#define QBOT_H

#include "framework\EliteMath\EMath.h"
#include "framework\EliteRendering\ERenderingTypes.h"
#include "Food.h"
#include "projects/Shared/BaseAgent.h"
#include "projects/Shared/NavigationColliderElement.h"

class QBot : public BaseAgent
{
public:
	QBot(float x,
		float y,
		float fov,
		float sFov,
		float angle,
		int memorySize,
		int nrInputs,
		int nrOutputs,
		bool useBias,
		float radius);

	virtual ~QBot() override;

	virtual void Update(vector<Food*>& food, float deltaTime);
	void Render(float deltaTime) override;
	
	bool IsAlive();
	void Reset();
	float CalculateFitness() const;
	//void MutateMatrix(Generation* gen, Elite::FMatrix& matrix, float mutationRate, float mutationAmplitude);
	void Reinforcement(float factor, int memory);
	float CalculateInverseDistance(float realDist);

	void SetObstacles(const std::vector<NavigationColliderElement*>& obstacles)
	{
		m_vNavigationColliders = obstacles;
	}

private:
	std::vector<NavigationColliderElement*> m_vNavigationColliders = {nullptr};
	float m_Radius;

	Elite::Vector2 m_Location; //TODO: maybe remove location and use get position
	Elite::Vector2 m_StartLocation;
	float m_Angle;
	float m_FOV;
	float m_MaxDistance = 50.0f;
	float m_SFOV;
	Elite::Color m_AliveColor;
	Elite::Color m_DeadColor;
	float m_Speed = 30.0f;
	float m_Health{ 100.0f };
	bool m_Alive = true;
	float m_Age{ 0.0f };

	// fitness members
	float m_TimeOfDeath = 0;
	int m_FoodEaten = 0;

	vector<Food*> m_Visible;

	int m_NrOfInputs;
	bool m_UseBias;
	int m_NrOfOutputs;
	int m_MemorySize;

	// currentIndex stores the information at the current time.
	// instead of swapping or copying matrices, manipulate the currentIndex to
	// go back in time. currentIndex is updated at the start of the update method
	// so that the render method also has the correct currentIndex. But make sure
	// the matrices at 0 are also filled in, otherwise problems.
	int currentIndex{ -1 };
	Elite::FMatrix* m_StateMatrixMemoryArr;
	Elite::FMatrix* m_ActionMatrixMemoryArr;
	Elite::FMatrix m_BotBrain;
	Elite::FMatrix m_DeltaBotBrain;
	Elite::FMatrix m_SAngle;

	// Q-factors, enable usage for different learning parameters for positive or for negative reinforcement.
	float m_NegativeQBig{ -0.8f };
	float m_NegativeQ{ -0.4f };
	float m_NegativeQSmall{ -0.1f };
	float m_PositiveQSmall{ 0.000001f };
	float m_PositiveQ{ 0.1f };
	float m_PositiveQBig{ 0.5f };
	int m_CameCloseCounter{ 0 };

};

#endif
#pragma once
#include "../SteeringHelpers.h"
#include "FlockingSteeringBehaviors.h"
#include "../SpacePartitioning/SpacePartitioning.h"

class ISteeringBehavior;
class SteeringAgent;
class BlendedSteering;
class PrioritySteering;


//#define USE_SPACE_PARTITIONING

class Flock
{
public:
	Flock(
		int flockSize = 50,
		float worldSize = 100.f, 
		SteeringAgent* pAgentToEvade = nullptr, 
		bool trimWorld = false);

	~Flock();

	void Update(float deltaT);
	void SpacePartitioningUpdate(float deltaT);
	void NoSpacePartitioningUpdate(float deltaT);
	void UpdateAndRenderUI() ;
	void Render(float deltaT);

// #ifdef USE_SPACE_PARTITIONING
// 	const vector<SteeringAgent*>& GetNeighbors() const { return m_pPartitionedSpace->GetNeighbors(); }
// 	int GetNrOfNeighbors() const { return m_pPartitionedSpace->GetNrOfNeighbors(); }

//#else // No space partitioning
	void RegisterNeighbors(SteeringAgent* pAgent);
	int GetNrOfNeighbors() const { return m_NrOfNeighbors; }
	const vector<SteeringAgent*>& GetNeighbors() const { return m_Neighbors; }
// #endif // USE_SPACE_PARTITIONING

	Elite::Vector2 GetAverageNeighborPos() const;
	Elite::Vector2 GetAverageNeighborVelocity() const;

	void SetSeekTarget(TargetData target);
	void SetWorldTrimSize(float size) { m_WorldSize = size; }

private:
	//Datamembers
	int m_FlockSize = 0;
	vector<SteeringAgent*> m_Agents;
// 	
// #ifdef USE_SPACE_PARTITIONING
// 	CellSpace* m_pPartitionedSpace = nullptr;
// 	int m_NrOfCellsX{ 25 };
// 
// 	vector<Elite::Vector2> m_OldPositions = {};
// 
// #else // No space partitioning
	vector<SteeringAgent*> m_Neighbors;
//#endif // USE_SPACE_PARTITIONING

	bool m_TrimWorld = false;
	bool m_CanDebugRender = false;
	bool m_CanDebugRenderNeighborhood = false;
	bool m_UseSpacePartitioning = true;
	float m_WorldSize = 0.f;

	float m_NeighborhoodRadius = 5.f;
	int m_NrOfNeighbors = 0;

	SteeringAgent* m_pAgentToEvade = nullptr;
	
	//Steering Behaviors
	Separation* m_pSeparationBehavior = nullptr;
	Cohesion* m_pCohesionBehavior = nullptr;
	VelocityMatch* m_pVelMatchBehavior = nullptr;
	Seek* m_pSeekBehavior = nullptr;
	Wander* m_pWanderBehavior = nullptr;
	Evade* m_pEvadeBehavior = nullptr;

	BlendedSteering* m_pBlendedSteering = nullptr;
	PrioritySteering* m_pPrioritySteering = nullptr;

	float* GetWeight(ISteeringBehavior* pBehaviour);

	CellSpace* m_CellSpace;
	std::vector<Elite::Vector2> m_OldPos{};
private:
	Flock(const Flock& other);
	Flock& operator=(const Flock& other);
};
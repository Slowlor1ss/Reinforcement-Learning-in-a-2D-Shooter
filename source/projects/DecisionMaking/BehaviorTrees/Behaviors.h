/*=============================================================================*/
// Copyright 2020-2021 Elite Engine
/*=============================================================================*/
// Behaviors.h: Implementation of certain reusable behaviors for the BT version of the Agario Game
/*=============================================================================*/
#ifndef ELITE_APPLICATION_BEHAVIOR_TREE_BEHAVIORS
#define ELITE_APPLICATION_BEHAVIOR_TREE_BEHAVIORS
//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "framework/EliteMath/EMath.h"
#include "framework/EliteAI/EliteDecisionMaking/EliteBehaviorTree/EBehaviorTree.h"
#include "projects/Shared/Agario/AgarioAgent.h"
#include "projects/Shared/Agario/AgarioFood.h"
#include "projects/Movement/SteeringBehaviors/Steering/SteeringBehaviors.h"

//-----------------------------------------------------------------
// Behaviors
//-----------------------------------------------------------------

Elite::BehaviorState ChangeToWander(Elite::Blackboard* pBlackboard)
{
	AgarioAgent* pAgent{ nullptr };
	pBlackboard->GetData("Agent", pAgent);

	if (!pAgent)
	{
		return Elite::BehaviorState::Failure;
	}
	pAgent->SetToWander();
	return Elite::BehaviorState::Success;
}

Elite::BehaviorState ChangeToSeek(Elite::Blackboard* pBlackboard)
{
	AgarioAgent* pAgent{ nullptr };
	pBlackboard->GetData("Agent", pAgent);
	Elite::Vector2 target = Elite::Vector2();
	pBlackboard->GetData("Target", target);

	if (!pAgent)
	{
		return Elite::BehaviorState::Failure;
	}
	pAgent->SetToSeek(target);
	return Elite::BehaviorState::Success;
}

Elite::BehaviorState ChangeToEvade(Elite::Blackboard* pBlackboard)
{
	AgarioAgent* pAgent{ nullptr };
	pBlackboard->GetData("Agent", pAgent);
	AgarioAgent* fleeTarget{ nullptr };
	pBlackboard->GetData("AgentFleeTarget", fleeTarget);

	if (!pAgent || !fleeTarget)
	{
		return Elite::BehaviorState::Failure;
	}
	pAgent->SetToEvade(fleeTarget->GetPosition());
	return Elite::BehaviorState::Success;
}

//Elite::BehaviorState ChangeToEvadeAndAvoidEdge(Elite::Blackboard* pBlackboard)
//{
//	AgarioAgent* pAgent{ nullptr };
//	pBlackboard->GetData("Agent", pAgent);
//	AgarioAgent* fleeTarget{ nullptr };
//	pBlackboard->GetData("AgentFleeTarget", fleeTarget);
//	Elite::Vector2 target = Elite::Vector2();
//	pBlackboard->GetData("Target", target);
//
//	if (!pAgent || !fleeTarget)
//	{
//		return Elite::BehaviorState::Failure;
//	}
//	pAgent->SetToEvadeWhileAvoidingEdge(fleeTarget->GetPosition(), target);
//	return Elite::BehaviorState::Success;
//}

bool IsFoodCloseBy(Elite::Blackboard* pBlackboard)
{
	AgarioAgent* pAgent{ nullptr };
	pBlackboard->GetData("Agent", pAgent);

	std::vector<AgarioFood*>* foods;
	pBlackboard->GetData("FoodVec", foods);

	std::vector<AgarioAgent*>* agents;
	pBlackboard->GetData("AgentsVec", agents);

	//OPTIONAL check if data is ok
	if (foods->empty() || agents->empty())
	{
		return false;
	}

	//1 get the closest food
	AgarioFood* closestFood = (*foods)[0];
	float closestDistance{ Elite::DistanceSquared(pAgent->GetPosition(), closestFood->GetPosition()) };
	for (size_t i{ 0 }; i < foods->size(); ++i)
	{
		//calculate distance
		float distanceSquared{ Elite::DistanceSquared(pAgent->GetPosition(), (*foods)[i]->GetPosition()) };
		if (distanceSquared < closestDistance) //if smaller change
		{
			//a bit overkill but checking if were closer to this food then the other agents, because if were not there's most of the time no point in trying to get that food first
			float distanceFoodEnemy{ Elite::DistanceSquared((*foods)[i]->GetPosition(), (*agents)[0]->GetPosition()) };
			for (const auto agent : *agents)
			{
				const float newDistance{ Elite::DistanceSquared((*foods)[i]->GetPosition(), agent->GetPosition()) };
				if (distanceFoodEnemy > newDistance)
				{
					distanceFoodEnemy = newDistance;
				}
			}

			if (distanceSquared < distanceFoodEnemy)
			{
				closestFood = (*foods)[i];
				closestDistance = distanceSquared;
			}
		}
	}
	//2 check if closest food is in a certain range
	const float range{ 50.f + pAgent->GetRadius()};
	if (closestDistance < Elite::Square(range))
	{
		pBlackboard->ChangeData("Target", closestFood->GetPosition());
		return true;
	}
	return false;
}

//slightly nerfed version for the enemies as the spawn position of our agent is very bad
bool IsFoodCloseByEnemies(Elite::Blackboard* pBlackboard)
{
	AgarioAgent* pAgent{ nullptr };
	pBlackboard->GetData("Agent", pAgent);

	std::vector<AgarioFood*>* foods;
	pBlackboard->GetData("FoodVec", foods);

	//OPTIONAL check if data is ok
	if (foods->empty())
	{
		return false;
	}

	//1 get the closest food
	AgarioFood* closestFood = (*foods)[0];
	float closestDistance{ Elite::DistanceSquared(pAgent->GetPosition(), closestFood->GetPosition()) };
	for (size_t i{ 0 }; i < foods->size(); ++i)
	{
		//calculate distance
		float distanceSquared{ Elite::DistanceSquared(pAgent->GetPosition(), (*foods)[i]->GetPosition()) };
		if (distanceSquared < closestDistance) //if smaller change
		{
			closestFood = (*foods)[i];
			closestDistance = distanceSquared;
		}
	}
	//2 check if closest enemy is in a certain range
	const float range{ 10.f - pAgent->GetRadius()};
	if (closestDistance < Elite::Square(range))
	{
		pBlackboard->ChangeData("Target", closestFood->GetPosition());
		return true;
	}
	return false;
}

bool IsBiggerEnemyClose(Elite::Blackboard* pBlackboard)
{
	AgarioAgent* pAgent{ nullptr };
	pBlackboard->GetData("Agent", pAgent);

	std::vector<AgarioAgent*>* agents;
	pBlackboard->GetData("AgentsVec", agents);

	//OPTIONAL check if data is ok
	if (agents->empty())
	{
		return false;
	}

	//1 get the closest enemy
	AgarioAgent* closestBiggerAgent = nullptr;
	float closestDistance{ FLT_MAX };
	for (size_t i{ 0 }; i < agents->size(); ++i)
	{
		//check if agent is bigger
		if ((*agents)[i]->GetRadius() > pAgent->GetRadius()+1)
		{
			//calculate distance
			const float distanceSquared{ DistanceSquared(pAgent->GetPosition(), (*agents)[i]->GetPosition()) };
			if (distanceSquared < closestDistance) //if smaller change
			{
				closestBiggerAgent = (*agents)[i];
				closestDistance = distanceSquared;
			}
		}
	}
	//2 check range
	if (closestBiggerAgent != nullptr)
	{
		const float range{ closestBiggerAgent->GetRadius() + pAgent->GetRadius() + 16.f }; //dynamic range
		if (closestDistance < Elite::Square(range))
		{
			pBlackboard->ChangeData("AgentFleeTarget", closestBiggerAgent);
			return true;
		}
	}
	return false;
}

bool IsSmallerEnemyClose(Elite::Blackboard* pBlackboard)
{
	AgarioAgent* pAgent{ nullptr };
	pBlackboard->GetData("Agent", pAgent);

	std::vector<AgarioAgent*>* agents;
	pBlackboard->GetData("AgentsVec", agents);

	//OPTIONAL check if data is ok
	if (agents->empty())
	{
		return false;
	}

	//1 get the closest enemy
	AgarioAgent* closestSmallerAgent = nullptr;
	float closestDistance{ DistanceSquared(pAgent->GetPosition(), (*agents)[0]->GetPosition()) };
	for (size_t i{ 0 }; i < agents->size(); ++i)
	{
		//check if agent is smaller
		if ((*agents)[i]->GetRadius()+2 < pAgent->GetRadius())
		{
			//calculate distance
			const float distanceSquared{ DistanceSquared(pAgent->GetPosition(), (*agents)[i]->GetPosition()) };
			if (distanceSquared <= closestDistance) //if smaller change
			{
				closestSmallerAgent = (*agents)[i];
				closestDistance = distanceSquared;
			}
		}
	}
	//2 check range
	if (closestSmallerAgent != nullptr)
	{
		const float range{ closestSmallerAgent->GetRadius() + pAgent->GetRadius() + 15.f };
		if (closestDistance < Elite::Square(range))
		{
			const bool isInFronOfPlayer{ Dot(closestSmallerAgent->GetPosition(), pAgent->GetLinearVelocity()) > 0 };
			const bool isEnemyMovingInSameDir{ Dot(closestSmallerAgent->GetLinearVelocity(), pAgent->GetLinearVelocity()) < 0 };
			if (!isEnemyMovingInSameDir && isInFronOfPlayer //check if the enemy is moving towards you otherwise you probably cant eat him as smaller == faster so you're too slow
				|| isEnemyMovingInSameDir && !isInFronOfPlayer) //or same direction but behind player
			{
				pBlackboard->ChangeData("Target", closestSmallerAgent->GetPosition());
				return true;
			}
		}
	}
	return false;
}

bool AvoidEdge(Elite::Blackboard* pBlackboard)
{
	AgarioAgent* pAgent{ nullptr };
	pBlackboard->GetData("Agent", pAgent);

	float worldSize;
	pBlackboard->GetData("WorldSize", worldSize);

	//OPTIONAL check if data is ok
	if (!pAgent)
	{
		return false;
	}

	//1 get the closest food
	const float Distance{ DistanceSquared(pAgent->GetPosition(), Elite::Vector2{0,0}) };

	//2 check range
	const float range{ worldSize - (worldSize * 0.1f + pAgent->GetRadius()) };
	if (Distance >= Elite::Square(range))
	{
		pBlackboard->ChangeData("Target", Elite::randomVector2(-worldSize + (worldSize * 0.7f), worldSize - (worldSize * 0.7f)));
		return true;
	}
	return false;
}

#endif
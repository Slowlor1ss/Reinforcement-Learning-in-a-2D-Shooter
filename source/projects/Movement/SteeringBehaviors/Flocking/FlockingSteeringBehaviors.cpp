#include "stdafx.h"
#include "FlockingSteeringBehaviors.h"
#include "TheFlock.h"
#include "../SteeringAgent.h"
#include "../SteeringHelpers.h"

//*******************
//COHESION (FLOCKING)
SteeringOutput Cohesion::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};
	if (m_pFlock->GetNeighbors().empty())
	{
		return steering;
	}
	
	steering.LinearVelocity = /*2**/ (m_pFlock->GetAverageNeighborPos() - pAgent->GetPosition());
	//steering.LinearVelocity.Normalize();
	//steering.LinearVelocity *= pAgent->GetMaxLinearSpeed();

	//debug
	if (pAgent->CanRenderBehavior())
	{
		DEBUGRENDERER2D->DrawCircle(m_pFlock->GetAverageNeighborPos(), 2, Elite::Color(1, 0, 1), 0.3f);
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5.f, { 1,0,1,0.5f }, 0.4f);
	}
	return steering;
}


//*********************
//SEPARATION (FLOCKING)
SteeringOutput Separation::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};
	if (m_pFlock->GetNeighbors().empty())
	{
		return steering;
	}
	
	Elite::Vector2 speed{};
	float distanceClosestEnemy{FLT_MAX};
	for (auto* pNeighbor : m_pFlock->GetNeighbors())
	{
		//speed += pNeighbor->GetPosition() - pAgent->GetPosition();
		const float tempDistance{ Elite::Distance(pNeighbor->GetPosition(), pAgent->GetPosition()) };
		if(tempDistance < distanceClosestEnemy)
		{
			distanceClosestEnemy=tempDistance;
			speed = pNeighbor->GetPosition() - pAgent->GetPosition();
		}
	}

	steering.LinearVelocity = speed;
	steering.LinearVelocity.Normalize();
	if (distanceClosestEnemy < m_SpeedEffectRadius)
		steering.LinearVelocity *= pAgent->GetMaxLinearSpeed();// / distanceClosestEnemy / m_SpeedEffectRadius;
	else
		steering.LinearVelocity *= pAgent->GetMaxLinearSpeed() / distanceClosestEnemy;
	steering.LinearVelocity *= -1;
	
	if (pAgent->CanRenderBehavior())
	{
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, steering.LinearVelocity.Magnitude(), { 1,1,0,0.5f }, 0.4f);
	}

	return steering;
}

//*************************
//VELOCITY MATCH (FLOCKING)
SteeringOutput VelocityMatch::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};
	steering.LinearVelocity = m_pFlock->GetAverageNeighborVelocity();

	if (pAgent->CanRenderBehavior())
	{
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, steering.LinearVelocity.Magnitude(), { 1,0,0,0.5f }, 0.4f);
	}
	
	return steering;
}
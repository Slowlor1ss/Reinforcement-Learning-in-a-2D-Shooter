//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

//Includes
#include "SteeringBehaviors.h"
#include "../SteeringAgent.h"

//SEEK
//****
SteeringOutput Seek::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};

	steering.LinearVelocity = m_Target.Position - pAgent->GetPosition();
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= pAgent->GetMaxLinearSpeed();

	//
	if (pAgent->CanRenderBehavior())
	{
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5.f, { 0,1,0,0.5f }, 0.4f);
	}
	
	return steering;
}

//FLEE
//****
SteeringOutput Flee::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	float distanceToTarget = Distance(pAgent->GetPosition(), m_Target.Position);
	if (distanceToTarget > m_FleeRadius)
	{
		return SteeringOutput(Elite::ZeroVector2, 0.f, false);
	}
	
	SteeringOutput steering = { Seek::CalculateSteering(deltaT, pAgent) };

	steering.LinearVelocity *= -1;
	

	if (pAgent->CanRenderBehavior())
	{
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5.f, { 0,1,0,0.5f }, 0.4f);
	}

	return steering;
}

//ARRIVE
//****
SteeringOutput Arrive::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = { Seek::CalculateSteering(deltaT, pAgent) };

	Elite::Vector2 toTarget = m_Target.Position - pAgent->GetPosition();
	const float distanceT = toTarget.Magnitude();

	if (distanceT < m_ArrivalRadius)
	{
		steering.LinearVelocity = Elite::Vector2{};
		return steering;
	}

	Elite::Vector2 velocity = toTarget;
	velocity.Normalize();
	if (distanceT < m_SlowRadius)
		velocity *= pAgent->GetMaxLinearSpeed() * distanceT / m_SlowRadius;
	else
		velocity *= pAgent->GetMaxLinearSpeed();

	steering.LinearVelocity = velocity;

	return steering;
}

//FACE
//****
SteeringOutput Face::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = { };

	//pAgent->GetRotation()
	pAgent->SetAutoOrient(false);
	//pAgent->SetRotation(AngleBetween(m_Target.Position, pAgent->GetDirection()));
	//pAgent->SetRotation(AngleBetween(pAgent->GetPosition(), m_Target.Position) - pAgent->GetRotation());
	
	//pAgent->SetRotation(Elite::GetOrientationFromVelocity(m_Target.Position-pAgent->GetPosition()));
	steering.AngularVelocity = Elite::GetOrientationFromVelocity(m_Target.Position - pAgent->GetPosition()) - pAgent->GetRotation();
	//pAgent->SetRotation(Elite::ToRadians(90));

	return steering;
}

//Wander
//****
SteeringOutput Wander::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = { };
	
	Elite::Vector2 displacement{0, m_Radius};
	RotateVector(displacement, m_WanderingAngle);
	m_WanderingAngle += Elite::randomFloat(m_MaxAngleChange) - (m_MaxAngleChange * 0.5f);

	const Elite::Vector2 pointInFrontOfAgent{ (
				pAgent->GetPosition() + (pAgent->GetDirection().GetNormalized() * (m_Radius + m_DistanceOffset))) };

	steering.LinearVelocity = displacement + pointInFrontOfAgent - pAgent->GetPosition();
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= pAgent->GetMaxLinearSpeed();

	//Debug
	if (pAgent->CanRenderBehavior())
	{
		
		DEBUGRENDERER2D->DrawDirection(pointInFrontOfAgent, displacement, m_Radius, { 1, 0, 0, 0.5f }, 0.4f);
		//DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), displacement+pointInFrontOfAgent, 1, { 1, 0, 1, 0.5f }, 0.4f);
		
		DEBUGRENDERER2D->DrawCircle(pointInFrontOfAgent
			, m_Radius, {0, 0, 1, 0.5f}, 0.4f);

		//DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5.f, { 0,1,0,0.5f }, 0.4f);
		
	}

	return steering;
}

void Wander::RotateVector(Elite::Vector2& vector, float angle)
{
	vector.x = cos(atan2(vector.y, vector.x) + angle) * vector.Magnitude();
	vector.y = sin(atan2(vector.y, vector.x) + angle) * vector.Magnitude();
	
// 	vector.x = ((x - dx) * cos(angle)) - ((y - dy) * sin(angle)) + dx
// 	y_rotated = ((x - dx) * sin(angle)) + ((y - dy) * cos(angle)) + dy
}

SteeringOutput Pursuit::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = { };

	const Elite::Vector2 distance{ m_Target.Position - pAgent->GetPosition() };
	//"how many updates the target needs to move from its current position to the pursuer position"
	const float updatesAhead = distance.Magnitude() / pAgent->GetMaxLinearSpeed();
	const Elite::Vector2 futurePosition = m_Target.Position + m_Target.LinearVelocity * updatesAhead;
	//return Seek(futurePosition);

	steering.LinearVelocity = futurePosition - pAgent->GetPosition();
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= pAgent->GetMaxLinearSpeed();

	return steering;
}

SteeringOutput Evade::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	const float distanceToTarget = Distance(pAgent->GetPosition(), m_Target.Position);
	
	//if not within radius
	if (distanceToTarget > m_EvadeRadius)
	{
		return SteeringOutput(Elite::ZeroVector2, 0.f, false);
	}
	
	SteeringOutput steering = { Pursuit::CalculateSteering(deltaT, pAgent) };
	
	steering.LinearVelocity *= -1;

	if (pAgent->CanRenderBehavior())
	{
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5.f, { 1,0,0,0.5f }, 0.4f);
	}
	
	return steering;
}

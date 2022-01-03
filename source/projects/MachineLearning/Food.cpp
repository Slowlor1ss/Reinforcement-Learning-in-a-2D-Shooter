#include "stdafx.h"
#include "Food.h"

Food::Food(float x, float y) : m_Location(x, y), m_Color(0.5f, 0.5f, 1) 
{

}

Vector2& Food::GetLocation() 
{
	return m_Location;
}

void Food::Render() 
{
	if (!m_IsEaten) {
		DEBUGRENDERER2D->DrawCircle(m_Location, .2f, m_Color,0.5f);
	}
}

void Food::Eat() 
{
	m_IsEaten = true;
	m_RespawnCounter = m_Respawn;
}

bool Food::IsEaten() 
{
	return m_IsEaten;
}

void Food::Update() 
{
	if (m_IsEaten) 
	{
		m_RespawnCounter--;
		if (m_RespawnCounter == 0) 
		{
			m_IsEaten = false;
		}
	}
}

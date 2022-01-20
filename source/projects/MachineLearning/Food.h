#ifndef FOOD
#define FOOD

#include "framework/EliteMath/EVector2.h"

using namespace Elite;
class Food
{
public:
	Food(float x, float y);
	
	Vector2& GetLocation();
	void Render();
	void Eat();
	bool IsEaten();
	void Update();
	void Respawn();
private:
	Vector2 m_Location;
	Color m_Color;
	bool m_IsEaten = false;
	int m_RespawnCounter{ 0 };
	int m_Respawn{ 40000 };
};
#endif
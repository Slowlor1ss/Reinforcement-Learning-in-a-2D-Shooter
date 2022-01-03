#ifndef FOOD
#define FOOD

#include "framework/EliteMath/EVector2.h"
#include "framework/EliteInterfaces/EIApp.h"
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
private:
	Vector2 m_Location;
	Color m_Color;
	bool m_IsEaten = false;
	int m_RespawnCounter{ 1000 };
	int m_Respawn{ 2500 };

};
#endif
#pragma once


class NavigationColliderElement;
class Food;
class QBot;

class Population
{
public:
	Population(int size, int nrOfFood, int nrOfInputs, int nrOfOutputs, bool bias);
	~Population();

	void Update(float deltaTime);
	void Render(float deltaTime) const;

private:
	float CalculateFitnessSum() const;
	float CalculateFitnessSum(unsigned int from, unsigned int to) const;
	Elite::FMatrix* SelectParentFPS(float sum) const;
	void SelectParentSUS() const;

	int m_Size;
	int m_NrOfFood;
	//bool m_AllBotsDead;
	int m_DeadCounter;
	int m_Generation;
	std::vector<QBot*> m_Bots;
	std::vector<vector<Food*>> m_Foodstuff;
	std::vector<NavigationColliderElement*> m_vNavigationColliders;
};


#ifndef MACHINE_LEARNING_APPLICATION_H
#define MACHINE_LEARNING_APPLICATION_H
//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "framework/EliteInterfaces/EIApp.h"
#include "QLearning.h"
#include "DynamicQLearning.h"
#include "projects/Shared/NavigationColliderElement.h"

//-----------------------------------------------------------------
// Application
//-----------------------------------------------------------------
class App_MachineLearning final : public IApp
{
public:
	//Constructor & Destructor
	App_MachineLearning() = default;
	virtual ~App_MachineLearning();

	//App Functions
	void Start() override;
	void Update(float deltaTime) override;
	void Render(float deltaTime) const override;

private:
	//Datamembers
	DynamicQLearning* m_pDynamicQEnv;
	QLearning* m_pGraph;

	//--Level--
	float m_TrimWorldSize = 70.f;
	std::vector<NavigationColliderElement*> m_vNavigationColliders = {};

	//C++ make the class non-copyable
	App_MachineLearning(const App_MachineLearning&) = delete;
	App_MachineLearning& operator=(const App_MachineLearning&) = delete;
};
#endif
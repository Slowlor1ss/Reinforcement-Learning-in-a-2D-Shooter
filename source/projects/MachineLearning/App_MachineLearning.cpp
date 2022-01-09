//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"
using namespace Elite;

//Includes
#include "App_MachineLearning.h"

//Statics

//Destructor
App_MachineLearning::~App_MachineLearning()
{
	//SAFE_DELETE(pPointer);
	//SAFE_DELETE(m_pGraph);
	SAFE_DELETE(m_pDynamicQEnv);

	//for (auto pNC : m_vNavigationColliders)
	//	SAFE_DELETE(pNC);
	//m_vNavigationColliders.clear();
}

//Functions
void App_MachineLearning::Start()
{
	//Initialization of your application. 
	//----------- CAMERA ------------
	DEBUGRENDERER2D->GetActiveCamera()->SetZoom(75.0f);
	DEBUGRENDERER2D->GetActiveCamera()->SetCenter(Elite::Vector2(0, 0));

	std::cout << "\n\n\n";
	FMatrix test{7,5};
	test.Randomize(0,1);
	test.Print();
	test.MakeFile("../Matrix.txt", std::ios::trunc);
	std::cout << "\n\n\n";
	test.parseFile("../Matrix.txt");
	test.Print();
	std::cout << "\n\n\n";

	////Create Boundaries
	//const float blockSize{ 5.0f };
	//const float hBlockSize{ blockSize / 2.0f };
	//m_vNavigationColliders.push_back(new NavigationColliderElement(Elite::Vector2(-m_TrimWorldSize - hBlockSize, 0.f), blockSize, (m_TrimWorldSize + blockSize) * 2.0f));
	//m_vNavigationColliders.push_back(new NavigationColliderElement(Elite::Vector2(m_TrimWorldSize + hBlockSize, 0.f), blockSize, (m_TrimWorldSize + blockSize) * 2.0f));
	//m_vNavigationColliders.push_back(new NavigationColliderElement(Elite::Vector2(0.0f, m_TrimWorldSize + hBlockSize), m_TrimWorldSize * 2.0f, blockSize));
	//m_vNavigationColliders.push_back(new NavigationColliderElement(Elite::Vector2(0.0f, -m_TrimWorldSize - hBlockSize), m_TrimWorldSize * 2.0f, blockSize));

	//m_vNavigationColliders.push_back(new NavigationColliderElement(Elite::Vector2(-70.0f	+ (36 / 2.f), -50.0f + (6 / 2.f)), 36, 6));
	//m_vNavigationColliders.push_back(new NavigationColliderElement(Elite::Vector2(-70.0f	+ (76 / 2.f), -28.0f + (6 / 2.f)), 76, 6));
	//m_vNavigationColliders.push_back(new NavigationColliderElement(Elite::Vector2(18.0f	+ (36 / 2.f), -28.0f + (6 / 2.f)), 36, 6));
	//m_vNavigationColliders.push_back(new NavigationColliderElement(Elite::Vector2(-70.0f	+ (36 / 2.f), -6.0f + (6 / 2.f)), 36, 6));
	//m_vNavigationColliders.push_back(new NavigationColliderElement(Elite::Vector2(-35.0f	+ (36 / 2.f), 15.0f + (6 / 2.f)), 36, 6));
	//m_vNavigationColliders.push_back(new NavigationColliderElement(Elite::Vector2(34.0f	+ (36 / 2.f), 15.0f + (6 / 2.f)), 36, 6));
	//m_vNavigationColliders.push_back(new NavigationColliderElement(Elite::Vector2(-70.0f	+ (84 / 2.f), 43.0f + (6 / 2.f)), 84, 6));
	//m_vNavigationColliders.push_back(new NavigationColliderElement(Elite::Vector2(34.0f	+ (36 / 2.f), 43.0f + (6 / 2.f)), 36, 6));

	// points_list = [(0, 1), (1, 5), (5, 6), (5, 4), (1, 2), (2, 3), (2, 7)]
	//m_pGraph = new QLearning(8,0,7);
	//m_pGraph->AddConnection(0, 1);
	//m_pGraph->AddConnection(1, 5);
	//m_pGraph->AddConnection(5, 6);
	//m_pGraph->AddConnection(5, 4);
	//m_pGraph->AddConnection(1, 2);
	//m_pGraph->AddConnection(2, 3);
	//m_pGraph->AddConnection(2, 7);

	//m_pGraph->SetLocation(0, Vector2(65, 0));
	//m_pGraph->SetLocation(1, Vector2(55, 40));
	//m_pGraph->SetLocation(2, Vector2(90, 80));
	//m_pGraph->SetLocation(3, Vector2(100, 50));
	//m_pGraph->SetLocation(4, Vector2(5, 80));
	//m_pGraph->SetLocation(5, Vector2(0, 45));
	//m_pGraph->SetLocation(6, Vector2(6, 0));
	//m_pGraph->SetLocation(7, Vector2(85, 120));

	//m_pGraph->AddKoboldLocation(4);
	//m_pGraph->AddKoboldLocation(5);
	//m_pGraph->AddKoboldLocation(6);

	//m_pGraph->AddTreasureLocation(2);


	//m_pGraph->PrintRewardMatrix();


	//m_pGraph->PrintQMatrix();
	//m_pGraph->TrainEnvironment();
	//m_pGraph->TrainWithEnvironment();

	m_pDynamicQEnv = new DynamicQLearning(50, 100, 16*2, 11, true);
	//m_pDynamicQEnv->GetQBot()->SetObstacles(m_vNavigationColliders);
	//m_pDynamicQEnv->GetQBot2()->SetObstacles(m_vNavigationColliders);
}

void App_MachineLearning::Update(const float deltaTime)
{
	//Update that is being called after the physics simulation
	//m_pCurrentGeneration->Update(deltaTime);
	//Get Mouse Clicks
	//if (INPUTMANAGER->IsMouseButtonUp(InputMouseButton::eMiddle))
	//{
	//	auto mouseData = INPUTMANAGER->GetMouseData(Elite::InputType::eMouseButton, Elite::InputMouseButton::eMiddle);
	//	Elite::Vector2 mouseTarget = DEBUGRENDERER2D->GetActiveCamera()->ConvertScreenToWorld(
	//		Elite::Vector2((float)mouseData.X, (float)mouseData.Y));
	//}
	//m_pGraph->Train();
	m_pDynamicQEnv->Update(deltaTime);
}

void App_MachineLearning::Render(const float deltaTime) const
{
	//DEBUGRENDERER2D->DrawSegment({ 0.0f, 0.0f }, { -30.0f, 80.0f }, Color(1.f, .5f, 0.f), -0.1f);
	//DEBUGRENDERER2D->DrawString({ 10.0f, 10.0f }, "TestString", Color(1.f, .5f, 0.f), -0.1f);
	//m_pCurrentGeneration->Render(deltaTime);
	//m_pGraph->Render(deltaTime);
	//for (const auto collider : m_vNavigationColliders)
	//{
	//	collider->RenderElement();
	//}
	m_pDynamicQEnv->Render(deltaTime);
}



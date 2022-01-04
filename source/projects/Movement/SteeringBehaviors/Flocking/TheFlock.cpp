#include "stdafx.h"
#include "TheFlock.h"

#include <numeric>

#include "../SteeringAgent.h"
#include "../Steering/SteeringBehaviors.h"
#include "../CombinedSteering/CombinedSteeringBehaviors.h"

//Constructor & Destructor
Flock::Flock(
	int flockSize /*= 50*/, 
	float worldSize /*= 100.f*/, 
	SteeringAgent* pAgentToEvade /*= nullptr*/, 
	bool trimWorld /*= false*/)

	: m_WorldSize{ worldSize }
	, m_FlockSize{ flockSize }
	, m_TrimWorld { trimWorld }
	, m_pAgentToEvade{pAgentToEvade}
	, m_NeighborhoodRadius{ 15 }
	, m_NrOfNeighbors{0}
{
	m_CellSpace = new CellSpace(m_WorldSize, m_WorldSize, 25, 25, m_FlockSize);
	
	m_Agents.resize(m_FlockSize);
	m_OldPos.resize(m_FlockSize);

	//initialize the flock and the memory pool
	m_pSeparationBehavior = new Separation(this);
	m_pVelMatchBehavior = new VelocityMatch(this);
	m_pCohesionBehavior = new Cohesion(this);
	m_pSeekBehavior = new Seek;
	m_pWanderBehavior = new Wander;
	
 	vector<BlendedSteering::WeightedBehavior> weightedSteeringBehaviors;
 	weightedSteeringBehaviors.push_back({m_pSeparationBehavior, 0.2f });
 	weightedSteeringBehaviors.push_back({m_pVelMatchBehavior, 0.2f });
 	weightedSteeringBehaviors.push_back({m_pCohesionBehavior, 0.2f });
 	weightedSteeringBehaviors.push_back({m_pSeekBehavior, 0.2f });
 	weightedSteeringBehaviors.push_back({m_pWanderBehavior, 0.2f });
 	m_pBlendedSteering = new BlendedSteering(weightedSteeringBehaviors);

// 	m_pAgentToEvade = new SteeringAgent;
// 	m_pAgentToEvade->SetBodyColor(Elite::Color(1.f, 0.f, 0.f));
// 	m_pAgentToEvade->SetAutoOrient(true);
// 	m_pAgentToEvade->SetSteeringBehavior(m_pWanderBehavior);
// 	m_pAgentToEvade->SetMaxLinearSpeed(50);
	
	//m_pEvadeBehavior = new Evade;
	//m_pPrioritySteering = new PrioritySteering({ m_pEvadeBehavior, m_pBlendedSteering});

	
	for (int i = 0; i < m_FlockSize; i++)
	{
		m_Agents[i]=new SteeringAgent;
		//m_Agents[i]->SetSteeringBehavior(m_pPrioritySteering);
		m_Agents[i]->SetSteeringBehavior(m_pBlendedSteering);
		m_Agents[i]->SetMass(1.0f);
		m_Agents[i]->SetMaxAngularSpeed(25.f);
		m_Agents[i]->SetMaxLinearSpeed(55.f);
		m_Agents[i]->SetAutoOrient(true);
		m_Agents[i]->SetPosition(Elite::randomVector2(0, worldSize));
	}
}

Flock::~Flock()
{
	SAFE_DELETE(m_pSeparationBehavior);
	SAFE_DELETE(m_pVelMatchBehavior);
	SAFE_DELETE(m_pCohesionBehavior);
	SAFE_DELETE(m_pEvadeBehavior);
	SAFE_DELETE(m_pWanderBehavior);
	SAFE_DELETE(m_pSeekBehavior);

	SAFE_DELETE(m_pBlendedSteering);
	SAFE_DELETE(m_pPrioritySteering);

	for(auto pAgent: m_Agents)
	{
		SAFE_DELETE(pAgent);
	}
	m_Agents.clear();

	SAFE_DELETE(m_pAgentToEvade);

	SAFE_DELETE(m_CellSpace);
}

void Flock::Update(float deltaT)
{
	m_UseSpacePartitioning ? SpacePartitioningUpdate(deltaT) : NoSpacePartitioningUpdate(deltaT);
}

void Flock::SpacePartitioningUpdate(float deltaT)
{
// 	m_pEvadeBehavior->SetTarget(TargetData(m_pAgentToEvade->GetPosition()));
// 	m_pAgentToEvade->Update(deltaT);
// 	m_pAgentToEvade->TrimToWorld({ 0,0 }, { m_WorldSize,m_WorldSize });

	// loop over all the agents
	for (size_t i = 0; i < m_Agents.size(); ++i)
	{
		// register its neighbors(-> memory pool is filled with neighbors of the currently evaluated agent)
// 		m_Neighbors.clear();
// 		m_NrOfNeighbors = 0;
		//get neighbors
		m_CellSpace->RegisterNeighbors(m_Agents[i], m_NeighborhoodRadius);
		m_NrOfNeighbors = m_CellSpace->GetNrOfNeighbors();
		m_Neighbors = m_CellSpace->GetNeighbors();

		// update it(-> the behaviors can use the neighbors stored in the pool, next iteration they will be the next agent's neighbors)
		m_Agents[i]->Update(deltaT);
		m_CellSpace->AddAgent(m_Agents[i]);
		m_CellSpace->UpdateAgentCell(m_Agents[i], m_OldPos[i]);

		m_OldPos[i] = m_Agents[i]->GetPosition();

	}


	// trim it to the world
	for (auto Agent : m_Agents)
	{
		if (m_TrimWorld)
		{
			Agent->TrimToWorld({ 0,0 }, { m_WorldSize,m_WorldSize });
		}
	}
}

void Flock::NoSpacePartitioningUpdate(float deltaT)
{
// 	m_pEvadeBehavior->SetTarget(TargetData(m_pAgentToEvade->GetPosition()));
// 	m_pAgentToEvade->Update(deltaT);
// 	m_pAgentToEvade->TrimToWorld({ 0,0 }, { m_WorldSize,m_WorldSize });

	// loop over all the agents
	for (size_t i = 0; i < m_Agents.size(); ++i)
	{
		// register its neighbors(-> memory pool is filled with neighbors of the currently evaluated agent)
		m_Neighbors.clear();
		m_NrOfNeighbors = 0;
		for (size_t j = 0; j < m_Agents.size(); ++j)
		{
			//don't make yourself a neighbor
			if (m_Agents[i] == m_Agents[j])
			{
				continue;
			}
			//check if it is a neighbor
			if (Elite::DistanceSquared(m_Agents[i]->GetPosition(), m_Agents[j]->GetPosition()) < m_NeighborhoodRadius * m_NeighborhoodRadius)
			{
				RegisterNeighbors(m_Agents[j]);
			}
		}
		// update it(-> the behaviors can use the neighbors stored in the pool, next iteration they will be the next agent's neighbors)
		m_Agents[i]->Update(deltaT);
	}


	// trim it to the world
	for (auto Agent : m_Agents)
	{
		if (m_TrimWorld)
		{
			Agent->TrimToWorld({0,0},{m_WorldSize,m_WorldSize});
		}
	}
}

void Flock::Render(float deltaT)
{
	//render the flock
	
	for (auto Agent : m_Agents)
	{
		Agent->Render(deltaT);
		Agent->SetRenderBehavior(m_CanDebugRender);
	}

// 	m_pAgentToEvade->Render(deltaT);
// 	m_pAgentToEvade->SetRenderBehavior(m_CanDebugRender);

	//DEBUG
	if (m_CanDebugRenderNeighborhood)
	{
		m_CellSpace->RenderCells(m_Agents.back(), m_NeighborhoodRadius);
		m_Agents.back()->SetRenderBehavior(true);
		DEBUGRENDERER2D->DrawCircle(m_Agents.back()->GetPosition(), m_NeighborhoodRadius, Elite::Color(1, 1, 1), 0.4f);
		for (auto* pNeighbors : m_Neighbors)
		{
			DEBUGRENDERER2D->DrawSolidCircle(pNeighbors->GetPosition(), 1, {}, Elite::Color(0, 1, 0), 0.3f);
		}		
	}

	if (m_TrimWorld)
	{
		std::vector<Elite::Vector2> points =
		{
			{ 0, m_WorldSize },
			{ m_WorldSize, m_WorldSize },
			{ m_WorldSize, 0 },
			{ 0, 0 }
		};
		DEBUGRENDERER2D->DrawPolygon(&points[0], 4, { 1,0,0,1 }, 0.4f);
	}
}

void Flock::RegisterNeighbors(SteeringAgent* pAgent)
{
	m_Neighbors.push_back(pAgent);
	++m_NrOfNeighbors;
}

void Flock::UpdateAndRenderUI()
{
	//Setup
	int menuWidth = 235;
	int const width = DEBUGRENDERER2D->GetActiveCamera()->GetWidth();
	int const height = DEBUGRENDERER2D->GetActiveCamera()->GetHeight();
	bool windowActive = true;
	ImGui::SetNextWindowPos(ImVec2(static_cast<float>(width) - menuWidth - 10, 10));
	ImGui::SetNextWindowSize(ImVec2(static_cast<float>(menuWidth), static_cast<float>(height) - 20));
	ImGui::Begin("Gameplay Programming", &windowActive, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
	ImGui::PushAllowKeyboardFocus(false);

	//Elements
	ImGui::Text("CONTROLS");
	ImGui::Indent();
	ImGui::Text("LMB: place target");
	ImGui::Text("RMB: move cam.");
	ImGui::Text("Scrollwheel: zoom cam.");
	ImGui::Unindent();

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
	ImGui::Spacing();

	ImGui::Text("STATS");
	ImGui::Indent();
	ImGui::Text("%.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
	ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
	ImGui::Unindent();

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::Text("Flocking");
	ImGui::Spacing();

	ImGui::Checkbox("Debug Rendering", &m_CanDebugRender);
	ImGui::Checkbox("Debug Rendering neighborhood", &m_CanDebugRenderNeighborhood);
 	ImGui::Checkbox("Use Space Partitioning", &m_UseSpacePartitioning);
// 	ImGui::Checkbox("Trim World", &m_TrimWorld);
// 	if (m_TrimWorld)
// 	{
// 		ImGui::SliderFloat("Trim Size", &m_WorldSize, 0.f, 500.f, "%1.");
// 	}
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();

	ImGui::Text("Behavior Weights");
	ImGui::Spacing();
	
	ImGui::SliderFloat("Separation",
		&m_pBlendedSteering->GetWeightedBehaviorsRef()[0].weight, 0.f, 1.f, "%.2");
	ImGui::SliderFloat("VelMatch",
		&m_pBlendedSteering->GetWeightedBehaviorsRef()[1].weight, 0.f, 1.f, "%.2");
	ImGui::SliderFloat("Cohesion",
		&m_pBlendedSteering->GetWeightedBehaviorsRef()[2].weight, 0.f, 1.f, "%.2");
	ImGui::SliderFloat("Seek",
		&m_pBlendedSteering->GetWeightedBehaviorsRef()[3].weight, 0.f, 1.f, "%.2");
	ImGui::SliderFloat("Wander",
		&m_pBlendedSteering->GetWeightedBehaviorsRef()[4].weight, 0.f, 1.f, "%.2");


	//End
	ImGui::PopAllowKeyboardFocus();
	ImGui::End();

}

Elite::Vector2 Flock::GetAverageNeighborPos() const
{
	//when there are no neighbors
	if (m_Neighbors.empty())
	{
		return Elite::Vector2{};
	}
	//vector<Elite::Vector2> positions{};
	Elite::Vector2 sum;
	
	for (auto* pNeighbor : m_Neighbors)
	{
		sum += pNeighbor->GetPosition();
		//positions.push_back(pNeighbor->GetPosition());
	}
	
	return sum / static_cast<float>(m_NrOfNeighbors);
	//return Elite::Vector2{ std::accumulate(positions.begin(), positions.end(), Elite::Vector2{}) / positions.size() };
}

Elite::Vector2 Flock::GetAverageNeighborVelocity() const
{
	//when there are no neighbors
	if (m_Neighbors.empty())
	{
		return Elite::Vector2{};
	}
	Elite::Vector2 sum;

	for (auto* pNeighbor : m_Neighbors)
	{
		sum += pNeighbor->GetLinearVelocity();
	}

	return sum / static_cast<float>(m_NrOfNeighbors);
}

void Flock::SetSeekTarget(TargetData target)
{
	//set target for Seek behavior
 	m_pSeekBehavior->SetTarget(target);
}

float* Flock::GetWeight(ISteeringBehavior* pBehavior) 
{
	if (m_pBlendedSteering)
	{
		auto& weightedBehaviors = m_pBlendedSteering->GetWeightedBehaviorsRef();
		auto it = find_if(weightedBehaviors.begin(),
			weightedBehaviors.end(),
			[pBehavior](BlendedSteering::WeightedBehavior el)
			{
				return el.pBehavior == pBehavior;
			}
		);

		if(it!= weightedBehaviors.end())
			return &it->weight;
	}

	return nullptr;
}

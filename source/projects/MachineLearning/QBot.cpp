#include "stdafx.h"
#include "QBot.h"
#include "Food.h"

// for setting the precision in cout for floating points.
#include <iomanip>

QBot::QBot(float x,
	float y,
	float fov,
	float sFov,
	float angle,
	int memorySize,
	int nrInputs,
	int nrOutputs,
	bool useBias,
	float radius) : BaseAgent(radius),
	m_Location(x, y), m_StartLocation(x, y), m_FOV(fov), m_SFOV(sFov), m_Angle(angle),
	//m_AliveColor(randomFloat(), randomFloat(), randomFloat()),
	m_AliveColor(0, 0.6f, 0.4f),
	m_DeadColor(.75f, 0.1f, .2f),
	m_NrOfInputs(nrInputs),
	m_NrOfOutputs(nrOutputs),
	m_MemorySize(memorySize),
	m_UseBias(useBias),
	m_BotBrain(nrInputs + (useBias ? 1 : 0), nrOutputs),
	m_DeltaBotBrain(nrInputs + (useBias ? 1 : 0), nrOutputs),
	m_SAngle(1, nrOutputs),
	m_Radius(radius)
{
	float start = -m_SFOV / 2;
	float step = m_SFOV / (nrOutputs - 1);
	for (int i = 0; i < nrOutputs; ++i)
	{
		float value = start + i * step;
		m_SAngle.Set(0, i, value);
	}

	m_ActionMatrixMemoryArr = new Elite::FMatrix[m_MemorySize];
	m_StateMatrixMemoryArr = new Elite::FMatrix[m_MemorySize];

	for (int i = 0; i < m_MemorySize; ++i)
	{
		m_StateMatrixMemoryArr[i].Resize(1, m_NrOfInputs + (m_UseBias ? 1 : 0));
		m_ActionMatrixMemoryArr[i].Resize(1, m_NrOfOutputs);
	}
	m_BotBrain.Randomize(-1.0f, 2.0f);
	if (m_UseBias) {
		m_BotBrain.SetRowAll(m_NrOfInputs, -10.0f);
	}

	//m_BotBrain.Print();

	SetPosition(m_Location);
}

QBot::~QBot() 
{
	delete[] m_ActionMatrixMemoryArr;
	m_ActionMatrixMemoryArr = nullptr;
	delete[] m_StateMatrixMemoryArr;
	m_StateMatrixMemoryArr = nullptr;
}

void QBot::Update(vector<Food*>& foodList, float deltaTime)
{
	currentIndex = (currentIndex + 1) % m_MemorySize;
	if (!m_Alive) {
		return;
	}
	m_Age += deltaTime;

	m_Visible.clear();
	const Elite::Vector2 dir(cos(m_Angle), sin(m_Angle));
	const float angleStep = m_FOV / (m_NrOfInputs/2);
	//const float angleStep = m_FOV / m_NrOfInputs;
	m_StateMatrixMemoryArr[currentIndex].SetAll(0.0);
	bool cameClose = false;
	for (Food* food : foodList) {
		if (food->IsEaten()) {
			continue;
		}
		Vector2 foodLoc = food->GetLocation();
		Vector2 foodVector = foodLoc - (m_Location - dir * 10);
		const float dist = (foodLoc-m_Location).Magnitude();
		if (dist > m_MaxDistance) {
			continue;
		}
		foodVector *= 1 / dist;


		const float angle = AngleBetween(dir, foodVector);
		if (angle > -m_FOV / 2 && angle < m_FOV / 2) {
			bool isBehindWall{ false };
			for (size_t i{4}; i < m_vNavigationColliders.size(); ++i)
			{
				if(m_vNavigationColliders[i]->Intersection(m_Location, foodLoc))
				{
					isBehindWall = true;
					break;
				}
			}
			if (isBehindWall)
			 continue;

			m_Visible.push_back(food);

			int index = static_cast<int>((angle + m_FOV / 2) / angleStep);
			float invDist = CalculateInverseDistance(dist);
			float currentDist = m_StateMatrixMemoryArr[currentIndex].Get(0, index);
			if (invDist > currentDist) {
				m_StateMatrixMemoryArr[currentIndex].Set(0, index, invDist);
			}
		}
		else if (dist < 10.0f + m_Radius) {
			cameClose = true;
		}


		if (dist < 2.0f + m_Radius) {
			food->Eat();
			m_CameCloseCounter = 50;
			m_FoodEaten++;
			m_Health += 30.0f;
			Reinforcement(m_PositiveQBig,m_MemorySize);
		}
	}

	bool StayedAwayFromWalls{true};
	bool NoWallsInFov{true};
	for (const auto obstacle : m_vNavigationColliders)
	{
		Vector2 obstacleVector = obstacle->GetClosestPoint(m_Location) - (m_Location - dir * 10);
		const float dist = obstacle->DistancePointRect(m_Location);
		if (dist > m_MaxDistance) {
			continue;
		}
		obstacleVector *= 1 / dist;

		const float angle = AngleBetween(dir, obstacleVector);
		if (angle > -m_FOV / 2 && angle < m_FOV / 2) {
			//m_Visible.push_back(food);
			NoWallsInFov = false;
			int index = (m_NrOfInputs*0.5f) + static_cast<int>((angle + m_FOV / 2) / angleStep);
			//int index = static_cast<int>((angle + m_FOV / 2) / angleStep);
			//experimental invert index
			//index = m_StateMatrixMemoryArr[currentIndex].GetNrOfColumns() - 1 - index;
			//float invDist{};
			//if (index == static_cast<int>((m_StateMatrixMemoryArr[currentIndex].GetNrOfColumns() - 1) * 0.5f))
			//	invDist = -99;
			//else
			float invDist = CalculateInverseDistance(dist);
			float currentDist = m_StateMatrixMemoryArr[currentIndex].Get(0, index);
			if (invDist > currentDist) {
				m_StateMatrixMemoryArr[currentIndex].Set(0, index, invDist);
			}
		}

		if (dist < 10.f + m_Radius) {
			StayedAwayFromWalls = false;
		}
		if (dist < 0.5f + m_Radius) {
			//m_Health -= 5.f;
			Reinforcement(m_NegativeQ, m_MemorySize);
			break;
		}
	}

	if (NoWallsInFov)
	{
		Reinforcement(m_PositiveQSmall*10, m_MemorySize);
	}
	else if (StayedAwayFromWalls)
	{
		Reinforcement(m_PositiveQSmall, m_MemorySize);
	}

	if (m_CameCloseCounter > 0) {
		m_CameCloseCounter--;
	}

	//Add a negative reinforcement if the bot near misses food (only every 50 frames at max)
	if (cameClose && m_CameCloseCounter == 0) {
		Reinforcement(m_NegativeQSmall,m_MemorySize);
		m_CameCloseCounter = 50;
	}

	m_StateMatrixMemoryArr[currentIndex].Set(0, m_NrOfInputs, 1); //bias
	m_StateMatrixMemoryArr[currentIndex].MatrixMultiply(m_BotBrain, m_ActionMatrixMemoryArr[currentIndex]);
	m_ActionMatrixMemoryArr[currentIndex].Sigmoid();

	int r, c;
	float max = m_ActionMatrixMemoryArr[currentIndex].Max(r, c);

	const float dAngle = m_SAngle.Get(0, c);
	m_Angle += dAngle * deltaTime;
	//TODO: also implement a accelerate and decelerate
	const Elite::Vector2 newDir(cos(m_Angle), sin(m_Angle));
	SetPosition(GetPosition() + newDir * m_Speed * deltaTime);
	m_Location = GetPosition();
	//m_Location += newDir * m_Speed*deltaTime;

	m_Health -= 0.1f;
	if (m_Health < 0) {
		// update the bot brain, something went wrong.
		Reinforcement(m_NegativeQBig,m_MemorySize);
		m_Alive = false;
		CalculateFitness();
		//m_Health = 100.0f;
		//m_Location = m_StartLocation;
		//SetPosition(m_StartLocation);
		
		//cout << "Died after "<< std::setprecision(4) << m_Age << " seconds." << endl;
		//m_Age = 0;
	}
	//DEBUGRENDERER2D->GetActiveCamera()->SetCenter(m_Location);

	//auto linVel = GetLinearVelocity();
	auto steeringForce = /*output.LinearVelocity*/newDir * m_Speed;// -linVel;
	auto acceleration = steeringForce / GetMass();

	//SetLinearVelocity(linVel + (acceleration * deltaTime));

	//Angular Movement
	//****************
	if (true/*m_AutoOrient*/)
	{
		auto desiredOrientation = Elite::GetOrientationFromVelocity(GetLinearVelocity());
		SetRotation(desiredOrientation);
	}
	else
	{
		//if (output.AngularVelocity > m_MaxAngularSpeed)
		//	output.AngularVelocity = m_MaxAngularSpeed;
		//SetAngularVelocity(output.AngularVelocity);
	}

	//m_BotBrain.Print();
}

void QBot::Render(float deltaTime) {
	Elite::Vector2 dir(cos(m_Angle), sin(m_Angle));
	Elite::Vector2 leftVision(cos(m_Angle + m_FOV / 2), sin(m_Angle + m_FOV / 2));
	Elite::Vector2 rightVision(cos(m_Angle - m_FOV / 2), sin(m_Angle - m_FOV / 2));

	Elite::Vector2 perpDir(-dir.y, dir.x);

	Color color = m_DeadColor;
	if (m_Alive) {
		color = m_AliveColor;
	}

	Color rayColor = {1, 0, 0};
	Color dirRayColor = {0, 1, 0};
	//DEBUGRENDERER2D->DrawSolidCircle(m_Location, 2, dir, c);
	//if (m_Alive) {
	//	DEBUGRENDERER2D->DrawSegment(m_Location - 10 * dir, m_Location + m_MaxDistance * leftVision, c);
	//	DEBUGRENDERER2D->DrawSegment(m_Location - 10 * dir, m_Location + m_MaxDistance * rightVision, c);
	//}
	const float angleStep = m_FOV / (m_NrOfInputs / 2);
	for (int i = 0; i < m_NrOfInputs/2 + 1; ++i)
	{
		Elite::Vector2 vision(cos((m_Angle + m_FOV / 2)-angleStep*i), sin((m_Angle + m_FOV / 2)-angleStep*i));
		DEBUGRENDERER2D->DrawSegment(m_Location - 10 * dir, m_Location + m_MaxDistance * vision, rayColor);
	}

	//The angle its moving towards
	int r, c;
	float max = m_ActionMatrixMemoryArr[currentIndex].Max(r, c);
	const float dAngle = m_SAngle.Get(0, c);

	const Vector2 dDir(cos(dAngle), sin(dAngle));
	DEBUGRENDERER2D->DrawSegment(m_Location - 10 * dir, m_Location + m_MaxDistance * dDir, dirRayColor);

#ifdef _DEBUG
	DEBUGRENDERER2D->DrawString(m_Location, to_string(static_cast<int>(m_Health)).c_str());
#endif


	if (m_Alive) {
		for (Food* f : m_Visible) {
			Vector2 loc = f->GetLocation();
			DEBUGRENDERER2D->DrawCircle(loc, 2, color, 0.5f);
		}
	}

	// draw the vision
	for (int i = 0; i < m_NrOfInputs; ++i)
	{

		if (m_StateMatrixMemoryArr[currentIndex].Get(0, i) > 0.0f) {
			DEBUGRENDERER2D->DrawSolidCircle(m_Location - 2.5 * dir - perpDir * 2.0f * (i - m_NrOfInputs / 2.0f), 1, perpDir, m_AliveColor);
		}
		else {
			DEBUGRENDERER2D->DrawSolidCircle(m_Location - 3.0 * dir - perpDir * 2.0f * (i - m_NrOfInputs / 2.0f), 1, perpDir, m_DeadColor);
		}
	}

	char age[10];
	snprintf(age, 10, "%.1f seconds", m_Age);
	DEBUGRENDERER2D->DrawString(m_Location + m_MaxDistance * dir, age);
}

bool QBot::IsAlive() const
{
	return m_Alive;
}

void QBot::Reset() 
{
	m_Health = 100;
	//m_TimeOfDeath = 0;
	m_Alive = true;
	m_FoodEaten = 0;

	m_Age = 0;
	m_Location = m_StartLocation;
	SetPosition(m_StartLocation);


	//float startx = Elite::randomFloat(-50.0f, 50.0f);
	//float starty = Elite::randomFloat(-50.0f, 50.0f);
	//float startAngle = Elite::randomFloat(0, static_cast<float>(M_PI) * 2);
}

void QBot::CalculateFitness() 
{
	m_Fitness = m_FoodEaten + m_Age;//m_TimeOfDeath;
}

void QBot::MutateMatrix(float mutationRate, float mutationAmplitude) 
{
	for (int c = 0; c < m_BotBrain.GetNrOfColumns(); ++c) 
	{
		for (int r = 0; r < m_BotBrain.GetNrOfRows(); ++r)
		{
			if (randomFloat(0, 1) < mutationRate) 
			{
				const float update = randomFloat(-mutationAmplitude, mutationAmplitude);
				const float currentVal = m_BotBrain.Get(r, c);
				m_BotBrain.Set(r, c, currentVal + update);
			}
		}
	}
}

//TODO: go over the memory variable it supposed to be how much memory we take in account so how long ago the actions that we will change
void QBot::Reinforcement(const float factor, const int memory) const
{
	// go back in time, and reinforce (or inhibit) the weights that led to the right/wrong decision.
	m_DeltaBotBrain.SetAll(0);

	//TODO: clean this up
#pragma push_macro("disable_min")
#undef min
	const int min = std::min(m_MemorySize, memory);
#pragma pop_macro("disable_min")

	auto oneDivMem{ 1.f / m_MemorySize };

	for (int mi{0}; mi < min; ++mi)
	{
		const auto timeFactor = 1 / (1 + Square(mi));

		const auto actualIndex = currentIndex - mi; //% (currentIndex+1);
		if (actualIndex < 0)
			return;

		int rMax;
		int cMax;
		m_ActionMatrixMemoryArr[actualIndex].Max(rMax, cMax);

		const auto scVal = m_StateMatrixMemoryArr[actualIndex].GetNrOfColumns();
		for (int c{0}; c < scVal; ++c)
		{
			if(m_StateMatrixMemoryArr[actualIndex].Get(0, c) > 0)
			{
				m_DeltaBotBrain.Add(c, cMax, timeFactor * factor * scVal);

				int rcMax;
				do
				{
					rcMax = Elite::randomInt(m_DeltaBotBrain.GetNrOfColumns()-1);
				}
				while (rcMax == cMax);

				m_DeltaBotBrain.Add(c, rcMax, -timeFactor * factor * scVal);
			}

			m_DeltaBotBrain.ScalarMultiply(oneDivMem);
			m_BotBrain.Add(m_DeltaBotBrain);
		}
	}
}


float QBot::CalculateInverseDistance(const float realDist) const
{
	// version 1 
	//return m_MaxDistance - realDist;
	// version 2
	const float nDist = realDist / m_MaxDistance;
	const float invDistSquared = m_MaxDistance / (1 + nDist * nDist);
	return invDistSquared;
}

void QBot::UniformCrossover(QBot* otherBrain)
{
	m_BotBrain.UniformCrossover(otherBrain->m_BotBrain);
}



#include "stdafx.h"
#include "QLearning.h"
#include <stdio.h>

QLearning::QLearning(int nrOfLocations, int startIndex, int endIndex)
	:m_pRewardMatrix(new Elite::FMatrix(nrOfLocations, nrOfLocations)),
	m_pQMatrix(new Elite::FMatrix(nrOfLocations, nrOfLocations)),
	m_pTreasureMatrix(new Elite::FMatrix(nrOfLocations, nrOfLocations)),
	m_pKoboldMatrix(new Elite::FMatrix(nrOfLocations, nrOfLocations)),
	m_pEnvMatrix(new Elite::FMatrix(nrOfLocations, nrOfLocations)),
	m_StartIndex(startIndex),
	m_EndIndex(endIndex),
	m_NrOfLocations(nrOfLocations),
	m_pIndexBuffer(new int[nrOfLocations])
{
	m_Locations.resize(nrOfLocations);
	m_pRewardMatrix->SetAll(-1.0f);
	m_pQMatrix->SetAll(0.0f);
	m_pKoboldMatrix->SetAll(0.0f);
	m_pTreasureMatrix->SetAll(0.0f);
	m_pRewardMatrix->Set(endIndex, endIndex, 100);
}

QLearning::~QLearning() {
	SAFE_DELETE(m_pRewardMatrix);
	SAFE_DELETE(m_pQMatrix);
	SAFE_DELETE(m_pTreasureMatrix);
	SAFE_DELETE(m_pKoboldMatrix);
	SAFE_DELETE(m_pEnvMatrix);

	delete[] m_pIndexBuffer;
	m_pIndexBuffer = nullptr;
}

void QLearning::SetLocation(int index, Elite::Vector2 location)
{
	if (index < m_NrOfLocations) {
		m_Locations[index] = location;
	}
}

void QLearning::AddConnection(int from, int to)
{
	// ----------------------------------------------
	// connections are set in the m_pRewardMatrix !!!
	// ----------------------------------------------
	// set two cells corresponding to (from,to) and (to,from) to 0
	// unless the to is equal to the end index, then the (from,to) should be 100.
	// use the set method of the fmatrix class
	if(to != m_EndIndex)
	{
		m_pRewardMatrix->Set(from, to, 0);
		m_pRewardMatrix->Set(to, from, 0);
	}
	else
	{
		m_pRewardMatrix->Set(from, to, 100);
		m_pRewardMatrix->Set(to, from, 0);
	}
}

void QLearning::AddTreasureLocation(int loc) 
{
	m_TreasureLocations.push_back(loc);
}
void QLearning::AddKoboldLocation(int loc) 
{
	m_KoboldLocations.push_back(loc);
}

void QLearning::Render(float deltaTime)
{
	char buffer[10];
	Elite::Vector2 arrowPoints[3];
	for (int row = 0; row < m_pRewardMatrix->GetNrOfRows(); ++row)
	{
		for (int column = 0; column < m_pRewardMatrix->GetNrOfColumns(); ++column)
		{
			if (m_pRewardMatrix->Get(row, column) >= 0.f) {

				Elite::Vector2 start = m_Locations[row];
				Elite::Vector2 end = m_Locations[column];
				float length = start.Distance(end);


				Elite::Vector2 dir = end - start;
				dir.Normalize();
				Elite::Vector2 perpDir(dir.y, -dir.x);
				Elite::Vector2 tStart = start + perpDir * 2;
				Elite::Vector2 tEnd = end + perpDir * 2;

				Elite::Vector2 mid = (tEnd + tStart) * .5 + 5 * dir;

				arrowPoints[0] = mid + dir * 5;
				arrowPoints[1] = mid + perpDir * 1.5f;
				arrowPoints[2] = mid - perpDir * 1.5f;

				float qValue = m_pQMatrix->Get(row, column);
				float max = m_pQMatrix->Max();
				float ip = qValue / max;
				float ipOneMinus = 1 - ip;
				Elite::Color c;
				c.r = m_NoQConnection.r * ipOneMinus + m_MaxQConnection.r * ip;
				c.g = m_NoQConnection.g * ipOneMinus + m_MaxQConnection.g * ip;
				c.b = m_NoQConnection.b * ipOneMinus + m_MaxQConnection.b * ip;
				DEBUGRENDERER2D->DrawSegment(tStart, tEnd, c);
				DEBUGRENDERER2D->DrawSolidPolygon(&arrowPoints[0], 3, c, 0.5);
				snprintf(buffer, 10, "%.0f", qValue);
				DEBUGRENDERER2D->DrawString(mid + perpDir * 3, buffer);
			}
		}
	}

	int index = 0;


	for (Elite::Vector2 loc : m_Locations)
	{
		snprintf(buffer, 3, "%d", index);
		DEBUGRENDERER2D->DrawString(loc + Elite::Vector2(1.5f, 0), buffer);
		if (index == m_StartIndex)
		{


			DEBUGRENDERER2D->DrawSolidCircle(loc, 2.0f, Elite::Vector2(1, 0), m_StartColor, 0.5f);
		}
		else if (index == m_EndIndex) {
			DEBUGRENDERER2D->DrawSolidCircle(loc, 2.0f, Elite::Vector2(1, 0), m_EndColor, 0.5f);
		}
		else {
			DEBUGRENDERER2D->DrawSolidCircle(loc, 2.0f, Elite::Vector2(1, 0), m_NormalColor, 0.5f);
		}

		++index;
	}

}

void QLearning::PrintRewardMatrix()
{
	m_pRewardMatrix->Print();
}

void QLearning::PrintQMatrix()
{
	m_pQMatrix->Print();
}

int QLearning::SelectAction(int currentLocation)
{
	// Step 2 in the slides, select a to node via the reward matrix.
	// return this to-node as the result

	//m_pIndexBuffer.clear();
	//float max = m_pRewardMatrix->MaxOfRow(currentLocation);
	//
	//for (int column{}; column < m_pRewardMatrix->GetNrOfColumns(); ++column)
	//	if (m_pRewardMatrix->Get(currentLocation, column) == max)
	//	{
	//		m_pIndexBuffer.push_back(column);
	//	}
	//
	//return m_pIndexBuffer[rand() % m_pIndexBuffer.size()];

	float max = m_pRewardMatrix->MaxOfRow(currentLocation);
	int maxElements{0};
	for (int column{}; column < m_pRewardMatrix->GetNrOfColumns(); ++column)
		if (m_pRewardMatrix->Get(currentLocation, column) == max)
		{
			m_pIndexBuffer[maxElements] = column;
			++maxElements;
		}

	return m_pIndexBuffer[rand() % maxElements];
}

int QLearning::SelectActionWithEnv(int currentLocation)
{
	// not used in ac 2021-2022
	return 0;
}

float QLearning::Update(int currentLocation, int nextAction)
{

	// A : get the max q-value of the row nextAction in the Q matrix
	//get index of biggest value
	//
	//
	//auto it = std::max_element(m_pIndexBuffer.begin(), m_pIndexBuffer.end(), [&](const int a, const int b) {return m_pQMatrix->Get(nextAction, a) < m_pQMatrix->Get(nextAction, b); });
	//
	//// B gather the elements that are equal to this max in an index buffer.
	//std::vector<int> maxElementsIndexes;
	//while (it != m_pIndexBuffer.end())
	//{
	//	//save the index
	//	maxElementsIndexes.push_back(std::distance(m_pIndexBuffer.begin(), it));
	//	//find if there are more with this value
	//	it = std::find_if(std::next(it), m_pIndexBuffer.end(), [&](const int a) { return m_pQMatrix->Get(nextAction, a) == m_pQMatrix->Get(nextAction, *it); });
	//}
	

	//std functions impementation (commented)

	//std::vector<int> rowIndexes{};
	//for (size_t i{ 0 }; i < m_pRewardMatrix->GetNrOfColumns(); ++i)
	//{
	//	rowIndexes.push_back(i);
	//}
	//
	//auto it = std::max_element(rowIndexes.begin(), rowIndexes.end(), [&](const int a, const int b) {return m_pQMatrix->Get(nextAction, a) < m_pQMatrix->Get(nextAction, b); });
	//
	//// gather the elements that are equal to this max in an index buffer.
	//std::vector<int> maxElementsIndexes;
	//while (it != rowIndexes.end())
	//{
	//	//save the index
	//	maxElementsIndexes.push_back(std::distance(rowIndexes.begin(), it));
	//	//find if there are more with this value
	//	it = std::find_if(std::next(it), rowIndexes.end(), [&](const int a) { return m_pQMatrix->Get(nextAction, a) == m_pQMatrix->Get(nextAction, *it); });
	//}


	// what is the maximum of the next action in the q-matrix
	float max = m_pQMatrix->MaxOfRow(nextAction);
	// gather the elements that are equal to this max in an index buffer.
	int maxElements{};
	for (int column{}; column < m_pQMatrix->GetNrOfColumns(); ++column)
		if (m_pQMatrix->Get(nextAction, column) == max)
		{
			m_pIndexBuffer[maxElements] = column;
			++maxElements;
		}


	// can use m_pIndexBuffer if it suits you. Devise your own way if you don't like it.

	// C pick a random index from the index buffer and implement the update formula
	// for the q matrix. (slide 14)
	//const int maxIndex = maxElementsIndexes[rand() % maxElementsIndexes.size()];
	const int maxIndex = m_pIndexBuffer[rand() % maxElements];
	m_pQMatrix->Set(currentLocation, nextAction, m_pRewardMatrix->Get(currentLocation, nextAction) + m_Gamma * m_pQMatrix->Get(nextAction, maxIndex));

	// calculate the score of the q-matrix and return it. (slide 15)
	return 100 * m_pQMatrix->Sum()/m_pQMatrix->Max();
}

void QLearning::Train() {
	if (m_CurrentIteration == 0)
	{
		start = chrono::high_resolution_clock::now();
	}
	if (m_CurrentIteration < m_NrOfIterations)
	{

		int currentLocation = Elite::randomInt(m_NrOfLocations);
		int nextLocation = SelectAction(currentLocation);
		float score = Update(currentLocation, nextLocation);
		printf("Score %.2f\n", score);


		m_CurrentIteration++;

	}
	//else if (m_CurrentIteration == m_NrOfIterations) {
	//	//test from start point 0
	//	int location = m_StartIndex;
	//
	//	printf("start at %d\t", location);
	//
	//	m_pIndexBuffer.clear();
	//	// TODO : find the best path via the q-matrix.
	//	// uncomment the while loop when implementing, be careful for infinite loop.
	//	while (location != m_EndIndex)
	//	{
	//		// what is the maximum of the next action in the q-matrix
	//		float max = m_pQMatrix->MaxOfRow(location);
	//		// gather the elements that are equal to this max in an index buffer.
	//		int maxElements{};
	//		for (int column{}; column < m_pQMatrix->GetNrOfColumns(); ++column)
	//			if (m_pQMatrix->Get(location, column) == max)
	//			{
	//				m_pIndexBuffer.push_back(column);
	//				maxElements++;
	//			}
	//
	//
	//		// pick a random index from the index buffer.
	//		location = m_pIndexBuffer[rand() % maxElements];
	//
	//		printf("%d\t", location, '%d\n');
	//		m_pQMatrix->Print();
	//
	//
	//	}
	//	m_CurrentIteration++;
	else if (m_CurrentIteration == m_NrOfIterations) {
		stop = chrono::high_resolution_clock::now();
		//test from start point 0
		int location = m_StartIndex;

		printf("start at %d\t", location);
		
		std::vector<int> rowIndexes{};
		for (size_t i{ 0 }; i < m_pRewardMatrix->GetNrOfColumns(); ++i)
		{
			rowIndexes.push_back(i);
		}
		// TODO : find the best path via the q-matrix.
		// uncomment the while loop when implementing, be careful for infinite loop.
		while (location != m_EndIndex)
		{
			//index of max value
			auto it = std::max_element(rowIndexes.begin(), rowIndexes.end(), [&](const int a, const int b) {return m_pQMatrix->Get(location, a) < m_pQMatrix->Get(location, b); });
			// gather the elements that are equal to this max in an index buffer.
			std::vector<int> maxElementsIndexes;
			while (it != rowIndexes.end())
			{
				//save the index
				maxElementsIndexes.push_back(std::distance(rowIndexes.begin(), it));
				//find if there are more with this value
				it = std::find_if(std::next(it), rowIndexes.end(), [&](const int a) { return m_pQMatrix->Get(location, a) == m_pQMatrix->Get(location, *it); });
			}

			location = maxElementsIndexes[rand() % maxElementsIndexes.size()];

			printf("%d\t", location);
		}
		std::cout << "\n";
		m_pQMatrix->Print();
		auto duration = (stop - start);

		cout << "Time taken by function: "
			<< duration.count() << " microseconds" << endl;
		m_CurrentIteration++;
	}
}

void QLearning::TrainEnvironment() {
	
}



void QLearning::TrainWithEnvironment() {
	

}
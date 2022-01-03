#pragma once

namespace Elite
{
	template <class T_NodeType, class T_ConnectionType>
	class AStar
	{
	public:
		AStar(IGraph<T_NodeType, T_ConnectionType>* pGraph, Heuristic hFunction);

		// stores the optimal connection to a node and its total costs related to the start and end node of the path
		struct NodeRecord
		{
			T_NodeType* pNode = nullptr;
			T_ConnectionType* pConnection = nullptr;
			float costSoFar = 0.f; // accumulated g-costs of all the connections leading up to this one
			float estimatedTotalCost = 0.f; // f-cost (= costSoFar + h-cost)

			bool operator==(const NodeRecord& other) const
			{
				return pNode == other.pNode
					&& pConnection == other.pConnection
					&& costSoFar == other.costSoFar
					&& estimatedTotalCost == other.estimatedTotalCost;
			};

			bool operator<(const NodeRecord& other) const
			{
				return estimatedTotalCost < other.estimatedTotalCost;
			};
		};

		std::vector<T_NodeType*> FindPath(T_NodeType* pStartNode, T_NodeType* pDestinationNode);

		//for debugging
		std::vector<T_NodeType*> GetcList() const { return m_cList; }
		std::vector<T_NodeType*> GetoList() const { return m_oList; }
	
	private:
		float GetHeuristicCost(T_NodeType* pStartNode, T_NodeType* pEndNode) const;

		IGraph<T_NodeType, T_ConnectionType>* m_pGraph;
		Heuristic m_HeuristicFunction;
		
		//for debugging
		std::vector<T_NodeType*> m_oList;
		std::vector<T_NodeType*> m_cList;
	};

	template <class T_NodeType, class T_ConnectionType>
	AStar<T_NodeType, T_ConnectionType>::AStar(IGraph<T_NodeType, T_ConnectionType>* pGraph, Heuristic hFunction)
		: m_pGraph(pGraph)
		, m_HeuristicFunction(hFunction)
	{
	}

	template <class T_NodeType, class T_ConnectionType>
	std::vector<T_NodeType*> AStar<T_NodeType, T_ConnectionType>::FindPath(T_NodeType* pStartNode, T_NodeType* pGoalNode)
	{
		T_NodeType* pCopyGoalNode{ pGoalNode };

		vector<T_NodeType*> path;
		vector<NodeRecord> openList;
		vector<NodeRecord> closedList;
		NodeRecord currentRecord{pStartNode, nullptr, 0, GetHeuristicCost(pStartNode, pCopyGoalNode)};

		openList.push_back(currentRecord);
		
		while (!openList.empty())
		{

			//find lowest cost of node record in open list
			for (auto it = openList.begin(); it != openList.end(); ++it)
			{
				if (*it < currentRecord)
				{
					currentRecord = *it;
				}
				else if(it->estimatedTotalCost == currentRecord.estimatedTotalCost) //if they're equal
				{
					if(GetHeuristicCost(it->pNode, pCopyGoalNode) < GetHeuristicCost(currentRecord.pNode, pCopyGoalNode)) //compare h-cost
						currentRecord = *it;
				}
			}
			//if (currentRecord.pNode == pGoalNode)
			if (currentRecord.pConnection != nullptr && currentRecord.pConnection->GetTo() == pCopyGoalNode->GetIndex())
				break; //found end node

			for (auto con : m_pGraph->GetNodeConnections(currentRecord.pNode))
			{
				auto gCost = con->GetCost() + currentRecord.costSoFar;

				T_NodeType* pNextNode = m_pGraph->GetNode(con->GetTo());
				//bool dontAdd{ false };


				auto duplicateClosedListCon = std::find_if(closedList.begin(), closedList.end(), [&con](const NodeRecord& n) { return con->GetTo() == n.pNode->GetIndex(); });
				if (duplicateClosedListCon != closedList.end())
				{
					if (duplicateClosedListCon->costSoFar < gCost)
						continue;
					else
						closedList.erase(duplicateClosedListCon);
				}

				//for (auto it = closedList.begin(); it != closedList.end();)
				//{
				//	//if (it->pConnection != nullptr)
				//	if (pNextNode == it->pNode) //check if the connection goes to a node already in the list
				//	{
				//		if (it->costSoFar < gCost)
				//		{
				//			++it;
				//			dontAdd = true;
				//			continue;
				//		}
				//		else
				//		{
				//			it = closedList.erase(it);
				//			continue;
				//		}
				//	}
				//	++it;
				//}

				auto duplicateOpenListCon = std::find_if(openList.begin(), openList.end(), [&con](const NodeRecord& n) { return con->GetTo() == n.pNode->GetIndex(); });
				if (duplicateOpenListCon != openList.end())
				{
					if (duplicateOpenListCon->costSoFar < gCost)
						continue;
					else
						openList.erase(duplicateOpenListCon);
				}

 			//	if (!dontAdd)
 			//	{
				//	for (auto it = openList.begin(); it != openList.end();)
				//	{
				//		if (pNextNode == it->pNode) //check if the connection goes to a node already in the list
				//		{
				//			if (it->costSoFar < gCost)
				//			{
				//				++it;
				//				dontAdd = true;
				//				continue;
				//			}
				//			else
				//			{
				//				it = openList.erase(it);
				//				continue;
				//			}
				//		}
				//		++it;
				//	}
				//}

				//if (!dontAdd)
				//{
					NodeRecord adj{ pNextNode, con, gCost, gCost + GetHeuristicCost(pNextNode, pCopyGoalNode) };
					openList.push_back(adj);
				//}
			}

			
			for (auto it = openList.begin(); it != openList.end(); ++it)
			{
				if (it->pNode == currentRecord.pNode)
				{
					openList.erase(it);
					break;
				}
			}

			closedList.push_back(currentRecord);

			if (openList.empty())
			{
				auto smallest = closedList[1];
												//don't add first node and also no point in checking element 1 as that is the start value of our smallest
				for (auto it1 = closedList.begin()+2; it1 != closedList.end(); ++it1)
				{
					if (GetHeuristicCost(it1->pNode, pGoalNode) < GetHeuristicCost(smallest.pNode, pGoalNode)) //compare h-cost
						smallest = *it1;
				}
													//don't add first node
				//pCopyGoalNode = std::min_element(closedList.begin()+1, closedList.end())->pNode;
				pCopyGoalNode = smallest.pNode;
				closedList.clear();
				currentRecord = { pStartNode, nullptr, 0, GetHeuristicCost(pStartNode, pCopyGoalNode) };
				openList.push_back(currentRecord);
			}
			else
			{
			//choosing between these 2 doesn't change a lot the path will be as shot it just might look a bit different
				//currentRecord = *openList.begin();
				currentRecord = *(openList.end()-1);
			}
			
		}

		//closedList.erase(closedList.begin());
		//start tracing back from end node
		while (currentRecord.pNode != pStartNode)
		{
			for (const auto & cList : closedList)
			{
				if (currentRecord.pConnection != nullptr)
					if (cList.pNode->GetIndex() == currentRecord.pConnection->GetFrom())
					{
						path.push_back(currentRecord.pNode);
						currentRecord = cList;
					}
			}
		}


		//debugging

// 		for (const auto& cList : closedList)
// 		{
// 			path.push_back(cList.pNode);
// 		}

		m_cList.clear();
		m_oList.clear();
		for (const auto& cList : closedList)
			m_cList.push_back(cList.pNode);
		for (const auto& oList : openList)
			m_oList.push_back(oList.pNode);
		
		path.push_back(pStartNode);
		std::reverse(path.begin(), path.end()); //Reversing the path

		return path;
	}

	template <class T_NodeType, class T_ConnectionType>
	float Elite::AStar<T_NodeType, T_ConnectionType>::GetHeuristicCost(T_NodeType* pStartNode, T_NodeType* pEndNode) const
	{
		Vector2 toDestination = m_pGraph->GetNodePos(pEndNode) - m_pGraph->GetNodePos(pStartNode);
		return m_HeuristicFunction(abs(toDestination.x), abs(toDestination.y));
	}
}
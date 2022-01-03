#pragma once
#include <vector>
#include <iostream>
#include "framework/EliteMath/EMath.h"
#include "framework\EliteAI\EliteGraphs\ENavGraph.h"
#include "framework\EliteAI\EliteGraphs\EliteGraphAlgorithms\EAStar.h"

namespace Elite
{
	class NavMeshPathfinding
	{
	public:
		static std::vector<Elite::Vector2> FindPath(Elite::Vector2 startPos, Elite::Vector2 endPos, Elite::NavGraph* pNavGraph, std::vector<Elite::Vector2>& debugNodePositions, std::vector<Elite::Portal>& debugPortals)
		{
			//Create the path to return
			std::vector<Elite::Vector2> finalPath{};

			//Get the start and endTriangle
			auto startTriangle = pNavGraph->GetNavMeshPolygon()->GetTriangleFromPosition(startPos);
			auto endTriangle = pNavGraph->GetNavMeshPolygon()->GetTriangleFromPosition(endPos);
			if (startTriangle == nullptr || endTriangle == nullptr)
			{
				return finalPath;
			}
			if (startTriangle == endTriangle)
			{
				finalPath.push_back(endPos);
				return finalPath;
			}
			//We have valid start/end triangles and they are not the same
			//=> Start looking for a path
			//Copy the graph
			auto pNavGraphCopy = pNavGraph;
			//Create extra node for the Start Node (Agent's position
			NavGraphNode* startNode = new NavGraphNode(pNavGraphCopy->GetNextFreeNodeIndex(), -1, startPos);//
			pNavGraphCopy->AddNode(startNode);
			for(auto lineIdx : startTriangle->metaData.IndexLines)
			{
				int lineNodeIdx = pNavGraphCopy->GetNodeIdxFromLineIdx(lineIdx);
				
				if (lineNodeIdx != invalid_node_index)
				{
					auto posFrom = startNode->GetPosition();
					auto posTo = pNavGraphCopy->GetNodePos(lineNodeIdx);
					pNavGraphCopy->AddConnection(new GraphConnection2D( startNode->GetIndex(), lineNodeIdx, abs(Distance(posFrom, posTo))));
				}
			}
			//Create extra node for the endNode
			NavGraphNode* endNode = new NavGraphNode(pNavGraphCopy->GetNextFreeNodeIndex(), -1, endPos);//
			pNavGraphCopy->AddNode(endNode);
			for (auto lineIdx : endTriangle->metaData.IndexLines)
			{
				int lineNodeIdx = pNavGraphCopy->GetNodeIdxFromLineIdx(lineIdx);

				if (lineNodeIdx != invalid_node_index)
				{
					auto posFrom = endNode->GetPosition();
					auto posTo = pNavGraphCopy->GetNodePos(lineNodeIdx);
					pNavGraphCopy->AddConnection(new GraphConnection2D(endNode->GetIndex(), lineNodeIdx, abs(Distance(posFrom, posTo))));
				}
			}
			//Run A star on new graph
			auto pathfinder = AStar<NavGraphNode, GraphConnection2D>(pNavGraphCopy, Elite::HeuristicFunctions::Chebyshev);//
			auto path = pathfinder.FindPath(startNode, endNode);
			debugNodePositions.clear();
// 			for (auto node : path)
// 			{
// 				finalPath.push_back(node->GetPosition());
// 				//OPTIONAL BUT ADVICED: Debug Visualisation
// 				debugNodePositions.push_back(node->GetPosition());
// 			}
 			for (auto node : path)
			{
				//OPTIONAL BUT ADVICED: Debug Visualization
				debugNodePositions.push_back(node->GetPosition());
			}
			

			//Run optimizer on new graph, MAKE SURE the A star path is working properly before starting this section and uncommenting this!!!
			const auto portals = SSFA::FindPortals(path, pNavGraph->GetNavMeshPolygon());
			debugPortals = portals;
			finalPath = SSFA::OptimizePortals(portals);
			
			return finalPath;
		}
	};
}

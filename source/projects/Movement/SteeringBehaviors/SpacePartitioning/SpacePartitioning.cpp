#include "stdafx.h"
#include "SpacePartitioning.h"
#include "projects\Movement\SteeringBehaviors\SteeringAgent.h"

// --- Cell ---
// ------------
Cell::Cell(float left, float bottom, float width, float height)
{
	boundingBox.bottomLeft = { left, bottom };
	boundingBox.width = width;
	boundingBox.height = height;
}

std::vector<Elite::Vector2> Cell::GetRectPoints() const
{
	const auto left = boundingBox.bottomLeft.x;
	const auto bottom = boundingBox.bottomLeft.y;
	const auto width = boundingBox.width;
	const auto height = boundingBox.height;

	std::vector<Elite::Vector2> rectPoints =
	{
		{ left , bottom  },
		{ left , bottom + height  },
		{ left + width , bottom + height },
		{ left + width , bottom  },
	};

	return rectPoints;
}

// --- Partitioned Space ---
// -------------------------
CellSpace::CellSpace(float width, float height, int rows, int cols, int maxEntities)
	: m_SpaceWidth(width)
	, m_SpaceHeight(height)
	, m_NrOfRows(rows)
	, m_NrOfCols(cols)
	, m_Neighbors(maxEntities)
	, m_NrOfNeighbors(0)
{
	m_CellHeight = m_SpaceHeight / m_NrOfRows;
	m_CellWidth = m_SpaceWidth / m_NrOfCols;
	for (int i = 0; i < m_NrOfCols; i++)
	{
		for (int j = 0; j < m_NrOfRows; ++j)
		{
			float left = m_CellWidth * j;
			float bottom = m_CellHeight * i;

			m_Cells.emplace_back(left, bottom, m_CellWidth, m_CellHeight);
			//m_Cells.push_back({left, bottom, m_CellWidth, m_CellHeight});
		}

	}
}

void CellSpace::AddAgent(SteeringAgent* agent)
{
	const size_t index = PositionToIndex(agent->GetPosition());
	//if the agent goes to the bottom of the trim box the position will be out of range for a split second
	//to prevent trying to access a cell that's out of range we check this
	if (index >= m_Cells.size() || index < 0)	return;

	m_Cells[index].agents.remove(agent);
	m_Cells[index].agents.push_back(agent);
	//m_Cells[index].agents.unique();
}

void CellSpace::UpdateAgentCell( SteeringAgent* const& agent, const Elite::Vector2& oldPos)
{
	const size_t index = PositionToIndex(agent->GetPosition());
	const size_t indexOld = PositionToIndex(oldPos);
	//if the agent goes to the bottom of the trim box the position will be out of range for a split second
	//to prevent trying to access a cell thats out of range we check this
	if (index >= m_Cells.size() || index < 0)
	{
		if (indexOld >= m_Cells.size() || indexOld < 0)	return;
		m_Cells[indexOld].agents.remove(agent);
		return;
	}
	if (indexOld >= m_Cells.size() || indexOld < 0)	return;

	if (index != indexOld)
	{
		m_Cells[indexOld].agents.remove(agent);
	}
}

void CellSpace::RegisterNeighbors(SteeringAgent* const& agent, const float queryRadius)
{
	m_Neighbors.clear();
	m_NrOfNeighbors = 0;
	const auto pos = agent->GetPosition();
	std::vector<int> cellIndexes{ GetOverlappingCells(pos,queryRadius) };

	for (int i : cellIndexes)
	{
		auto &agents = m_Cells[i].agents;

		for (auto* pAgent : agents)
		{
			//don't add the agent them self
			if (pAgent == agent) continue;
			if (DistanceSquared(pAgent->GetPosition(), pos) < queryRadius * queryRadius)
 			{
				m_Neighbors.push_back(pAgent);
				++m_NrOfNeighbors;
			}
		}
	}

// 	for (size_t i = 0; i < cellIndexes.size(); i++)
// 	{
// 		DEBUGRENDERER2D->DrawPolygon(&(m_Cells[cellIndexes[i]].GetRectPoints())[0], 4, { 0,1,0,1 }, 0.4f);
//  	}
}

void CellSpace::RenderCells(SteeringAgent* const& agent, float queryRadius) const
{
	for (int i = 1; i < m_NrOfRows; ++i)
	{
		DEBUGRENDERER2D->DrawSegment({ 0, m_CellHeight * i }, { m_SpaceWidth, m_CellHeight * i }, { 1,0,0 });
	}
	for (int i = 1; i < m_NrOfCols; ++i)
	{
		DEBUGRENDERER2D->DrawSegment({ m_CellWidth * i , 0 }, { m_CellWidth * i, m_SpaceHeight }, { 1,0,0 });
	}

	for (int i = 0; i < m_NrOfRows; ++i)
	{
		for (int j = 0; j < m_NrOfCols; ++j)
		{
			DEBUGRENDERER2D->DrawString({ m_CellWidth * i , m_CellHeight * (j + 1) }, { std::to_string(m_Cells[(j * m_NrOfRows) + i].agents.size()).c_str() });
		}
	}

	std::vector<int> cellIndexes{ GetOverlappingCells(agent->GetPosition(), queryRadius) };
	for (int i : cellIndexes)
	{
		DEBUGRENDERER2D->DrawPolygon(&(m_Cells[i].GetRectPoints())[0], 4, { 0,1,0,1 }, 0.4f);
	}
}

int CellSpace::PositionToIndex(const Elite::Vector2& pos) const
{
	int i, j;
	// 	i = pos.x / m_SpaceWidth;
	// 	j = pos.y / m_SpaceWidth;
	i = static_cast<int>(Elite::Clamp(pos.x, 0.f, m_SpaceWidth - m_CellWidth) / m_CellWidth);
	j = static_cast<int>(Elite::Clamp(pos.y, 0.f, m_SpaceHeight - m_CellHeight) / m_CellHeight);
	return (j * m_NrOfRows) + i;
}

std::vector<int> CellSpace::GetOverlappingCells(const Elite::Vector2& pos, float radius) const
{
	std::vector<int> celIndexes{};
	//create the bounding box around the agent
	const float bottom = pos.y - radius;
	const float left = pos.x - radius;
	const float width = radius * 2;
	const float height = width;

	//get the row and column from the top right and bottom left and all the cells in that square are within our bounding box
	//doing it this way prevents us from having to loop over all cells and do a collision check here we just simply calculate 
	//two cells and take all the cells that form a square from those two
	const Elite::Vector2 rowColBottomLeft = GetRowCol({ left,bottom });
	const Elite::Vector2 rowColTopRight = GetRowCol({ left + width,bottom + height });

	//get the cells that are in the bounding box
	for (float i = rowColBottomLeft.y; i < rowColTopRight.y + 1; ++i)
	{
		for (float j = rowColBottomLeft.x; j < rowColTopRight.x + 1; ++j)
		{
			celIndexes.push_back(GetIndex({ j, i}));
		}
	}

	//return the indexes of the cells that are in the bounding box
	return celIndexes;
}

Elite::Vector2 CellSpace::GetRowCol(int idx) const
{
	int j = idx / m_NrOfCols;
	int i = idx - (j * m_NrOfCols);
	return {static_cast<float>(i), static_cast<float>(j)};
}

Elite::Vector2 CellSpace::GetRowCol(const Elite::Vector2& pos) const
{
	int i = static_cast<int>(Elite::Clamp(pos.x, 0.f, m_SpaceWidth - m_CellWidth) / m_CellWidth);
	int j = static_cast<int>(Elite::Clamp(pos.y, 0.f, m_SpaceHeight - m_CellHeight) / m_CellHeight);
	return { static_cast<float>(i), static_cast<float>(j) };
}

int CellSpace::GetIndex(Elite::Vector2 rowCol) const
{
	return static_cast<int>((rowCol.y * m_NrOfRows) + rowCol.x);
}

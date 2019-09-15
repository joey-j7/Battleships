#include "PathfindSystem.h"
#include <algorithm>
#include <climits>
#include <cassert>
#include <list>
#include "Path.h"
#include "PhysicsDebugDrawer.h"
#include "PathfindAgent.h"
#include "Transform.h"

// Reserved node index defined for unidentified/unknown node
#define NODE_INDEX_RESERVED USHRT_MAX

static unsigned Clamp(unsigned value, unsigned min, unsigned max)
{
	return std::min(std::max(value, min), max);
}

PathfindSystem::PathfindSystem(unsigned int a_uiMapSize, unsigned a_uiNodeScalar)
{
	// World size of the game 
	m_uWorldWidth = a_uiMapSize;
	m_uWorldDepth = a_uiMapSize;

	// Node scalar which indicates the node size
	// Node origin for the half of a nodes edge
	// Node inverse scalar for initalization of nodes position
	// this is for increasing performance rather than dividing
	m_uNodeScalar = a_uiNodeScalar;
	m_uNodeOrigin = m_uNodeScalar / 2;
	m_uInvNodeScalar = 1 / m_uNodeScalar;

	// Calculation of the total count of nodes on X axis
	// Calcualtion the inverse value of the total count on X axis
	m_uNodeCountX = (m_uWorldWidth / m_uNodeScalar);
	m_fInvNodeCountX = 1.f / (float)m_uNodeCountX;

	// Calculation of the total count of nodes on Y axis
	// Calcualtion the inverse value of the total count on Y axis
	m_uNodeCountZ = (m_uWorldDepth / m_uNodeScalar);
	m_fInvNodeCountZ = 1.f / (float)m_uNodeCountZ;

	// Total counts of nodes which needs to be created
	// Resizing the vector to incries performance of setting up grid
	unsigned nodecount = m_uNodeCountX * m_uNodeCountZ;
	m_vGrid.resize(nodecount);

	// Initializaiton of all the nodes within the grid
	// Setting the position of the node to the world position
	// Setting the isfree boolean to true by default
	for(unsigned a = 0 ; a != m_uNodeCountZ ; ++a)
	{
		for (unsigned i = 0; i != m_uNodeCountX; ++i)
		{
			unsigned nodeindex = (m_uNodeCountX * a) + i;
			GridPosition gridpos = { i, a };

			m_vGrid[nodeindex].v3Position = ConvertGridToPosition(gridpos);
			m_vGrid[nodeindex].bIsFree = true;

			for (unsigned dirindex = 0; dirindex != NodeDirections::E_DIRECTION_COUNT; ++dirindex)
				m_vGrid[nodeindex].uaNeighbours[dirindex] = ConvertDirectionToIndex(gridpos, (NodeDirections)dirindex);
		}
	}
}

bool PathfindSystem::CanProcessComponent(Component* a_pComponent)
{
	return a_pComponent->GetType() == Component::E_Pathfinding;
}

void PathfindSystem::ProcessComponents()
{
	Render();

	for (PathfindAgent* pAgent : m_vAgents)
	{
		if (!pAgent->HasDestination()) continue;

		const glm::vec3 start = pAgent->GetObject()->GetTransform()->GetWorldPosition();

		Path path;
		const bool found = FindPath(start, pAgent->GetDestination(), path);
		pAgent->OnPathReceived(found, path);
	}
}

void PathfindSystem::OnComponentDestroyed(Component* a_pComponent)
{
	std::vector<PathfindAgent*>::iterator iter = std::find(m_vAgents.begin(), m_vAgents.end(), a_pComponent);
	if (iter != m_vAgents.end())
		m_vAgents.erase(iter);
}

void PathfindSystem::OnComponentAdded(Component* a_pComponent)
{
	m_vAgents.push_back(static_cast<PathfindAgent*>(a_pComponent));
}

void PathfindSystem::Render()
{
#if DEBUG
	if (!PhysicsDebugDrawer::GetInstance()->DoesDrawDebug())
		return;

	// Height for rendering of the lines
	// Also the color is predifined which is red
	float debugheight = 2;
	btVector3 color = btVector3(1, 0, 0);

	// Loop through the grid nodes which is row major
	for(unsigned i = 0 ; i != m_vGrid.size() ; ++i)
	{
		// If the current node is traversable, render the lines
		if(m_vGrid[i].bIsFree == true)
		{
			// get a reference to the current node position
			const glm::vec3& NodePos = m_vGrid[i].v3Position;

			// Construction of the 4 lines of the current node 
			btVector3 linePoints[4];
			linePoints[0] = btVector3(NodePos.x - m_uNodeOrigin, debugheight, NodePos.z - m_uNodeOrigin);
			linePoints[1] = btVector3(linePoints[0].getX() + m_uNodeScalar, linePoints[0].getY(), linePoints[0].getZ());
			linePoints[2] = btVector3(linePoints[1].getX(), linePoints[1].getY(), linePoints[1].getZ() + m_uNodeScalar);
			linePoints[3] = btVector3(linePoints[0].getX(), linePoints[0].getY(), linePoints[0].getZ() + m_uNodeScalar);

			// For each line, use the physics debug drawer to render those lines
			for(unsigned a = 0 ; a != 4 ; ++a)
				PhysicsDebugDrawer::GetInstance()->drawLine(linePoints[a], linePoints[(a != 3) ? a + 1 : 0], color);
		}
	}
#endif 
}

bool PathfindSystem::FindPath(const glm::vec3& a_v3Start, const glm::vec3& a_v3End, Path& a_rPath)
{
	// Position correction if needed
	glm::vec3 startposition = CorrectPositionOnBounds(a_v3Start);
	glm::vec3 endposition = CorrectPositionOnBounds(a_v3End);
	
	// Create reference to the start and end node
	Node& startNode = GetNodeFromPosition(startposition);
	Node& endNode = GetNodeFromPosition(endposition);

	if (endNode.bIsFree == false)
		return false;

	// Create openlist to store nodes that still can be checked
	// Create closedlist to store nodes that can't be used
	std::list<Node*> openList;
	std::list<Node*> closedList;

	// Add the starting node to the openlist
	openList.push_back(&startNode);
	
	// Set the variables for the starting node
	startNode.fGCost = 0;
	startNode.fHCost = GetDistanceNodeToNode(startNode, endNode);
	startNode.fFCost = startNode.fGCost + startNode.fHCost;
	startNode.pParentNode = nullptr;

	// While the openlist is not empty, process the node with the lowest
	// predicted path cost. If openlist is empty en end node hasn't been
	// reached than no path can be established
	while(openList.size() != 0)
	{
		// Set the current node to the first within the list
		Node* currentNode = openList.front();

		// Loop through all nodes within the list to find the node 
		// with the lowest predicted path cost. If path costs are equal than
		// take the node with the lowest predicted cost from the current iterated
		// node and the current one and compare those
		for(auto it = openList.begin() ; it != openList.end() ; it++)
		{
			if ((*it)->fFCost < currentNode->fFCost || ((*it)->fFCost == currentNode->fFCost && (*it)->fHCost < currentNode->fHCost))
				currentNode = *it;
		}

		// Remove the current node from openlist
		// Add the current node to the closed list due to being
		// proccesed after this loop
		openList.remove(currentNode);
		closedList.push_back(currentNode);

		// Check if the current node is the actual end end node
		// if this is the case a path has been found
		if(currentNode == &endNode)
		{
			CreatePath(startposition, endposition, startNode, *currentNode, a_rPath);

			return true;
		}

		// Process all the neighbor nodes of the current node
		for(unsigned dirindex = 0 ; dirindex != NodeDirections::E_DIRECTION_COUNT ; ++dirindex)
		{
			// Check if the node index of the neighbor is valid
			if(IsNodeIndexValid(currentNode->uaNeighbours[dirindex]))
			{
				// Set pointer to the neighbor node
				Node* neighbour = &GetNodeFromIndex(currentNode->uaNeighbours[dirindex]);

				// If the neighbor is on the closed list set the process Boolean to false
				auto found = std::find(closedList.begin(), closedList.end(), neighbour);
				if (found == closedList.end() && neighbour->bIsFree)
				{
					// Set Boolean with information if the neighbor should be processed
					bool processNeighour = true;
					
					// If the neighbor node is diagonal from current node
					if(dirindex % 2)
					{
						Node* neighbourA = &GetNodeFromIndex(currentNode->uaNeighbours[dirindex - 1]);
						Node* neighbourB = &GetNodeFromIndex(currentNode->uaNeighbours[(dirindex == NodeDirections::E_DIRECTION_COUNT - 1) ? 0 : dirindex + 1]);
						
						if ((!neighbourA->bIsFree) || (!neighbourB->bIsFree))
							processNeighour = false;
					}
					
					// If the process Boolean is true at this point than process the neighbor
					if(processNeighour)
					{
						// Calculate the start cost of the neighbor node added to it the current cost from start(current node)
						int newMovementCost = currentNode->fGCost + GetDistanceNodeToNode(*currentNode, *neighbour);

						// Check if the neigbor node is already in the open list
						// Check if new movement cost (current movement cost) is lower than the neighbor
						// or if the neighbor is not on the openlist, update the neighbor
						auto found = std::find(openList.begin(), openList.end(), neighbour);
						if (newMovementCost < neighbour->fGCost || found == openList.end())
						{
							// Set the cost from start to neighbor in the neighbor
							// Set the predicted cost from the neigbor to the end point
							// Set the predicted total from parent nodes with predicted to end node
							// Set the parrent node of the neigbor to current one
							neighbour->fGCost = newMovementCost;
							neighbour->fHCost = GetDistanceNodeToNode(*neighbour, endNode);
							neighbour->fFCost = neighbour->fGCost + neighbour->fHCost;
							neighbour->pParentNode = currentNode;
						
							// If neigbor not on openlist than add it on the openlist
							if(found == openList.end())
								openList.push_back(neighbour);
						}
					}
				}
			}
		}
	}

	return false;
}

void PathfindSystem::AddStaticNoneTraversable(const glm::vec3 a_v3Min, const glm::vec3 a_v3Max)
{
	GenerateTraversable(a_v3Min, a_v3Max, false);
}

void PathfindSystem::AddStaticTraversable(const glm::vec3 a_v3Min, const glm::vec3 a_v3Max)
{
	GenerateTraversable(a_v3Min, a_v3Max, true);
}

unsigned PathfindSystem::GetGridWidth() const
{
	return m_uWorldWidth;
}

unsigned PathfindSystem::GetGridDepth() const
{
	return m_uWorldDepth;
}

void PathfindSystem::CreatePath(const glm::vec3& a_v3Start, const glm::vec3& a_v3End, Node& a_pStartNode, Node& a_pCurrentNode, Path& a_rPath)
{
	if (!a_pCurrentNode.pParentNode) return;

	// Setup reference to end node which is equal to current node pointer
	// Setup pointer to curr node for a loop later on
	// Setup of data wrapper to set data and push on in the path 
	Node& endNode = a_pCurrentNode; 
	Node* curr = &a_pCurrentNode;
	PathNode currPathNode;
			
	// If the start and end node are not the same than proceed
	if(&a_pStartNode != &endNode)
	{	
		// While not reached start node by walking through the nodes
		// through each current nodes parent
		while(curr != nullptr)
		{
			// If current node not is the start node
			if(curr != &a_pStartNode)
			{
				if (curr != &endNode && curr->pParentNode != &a_pStartNode)
				{
					glm::vec3 dirnormal;
							
					(curr->pParentNode != &a_pStartNode) ?
						dirnormal = glm::normalize(curr->v3Position - curr->pParentNode->v3Position) :
						dirnormal = glm::normalize(curr->v3Position - a_v3Start);
					
					const PathNode& lastpathnode = a_rPath.GetCurrentPathNode();
					glm::vec3 prevdirnormal = glm::normalize(lastpathnode.v3Position - curr->v3Position);
					
					if (prevdirnormal != dirnormal)
					{
						currPathNode.v3Position = curr->v3Position;
						currPathNode.v3RotationPoint = curr->v3Position - dirnormal * 3;
						a_rPath.AddPathNode(currPathNode);
					}
				}
				else if (curr == &endNode)
				{
					currPathNode.v3Position = a_v3End;
					currPathNode.v3RotationPoint = a_v3End - glm::normalize(a_v3End - curr->pParentNode->v3Position);
					a_rPath.AddPathNode(currPathNode);
				} 
				else if (curr->pParentNode == &a_pStartNode)
				{
					currPathNode.v3Position = curr->v3Position;
					currPathNode.v3RotationPoint = curr->v3Position - glm::normalize(curr->v3Position - a_v3Start) * 3;
					a_rPath.AddPathNode(currPathNode);
				}
			} 
					
			// Reset data information of the node and
			// set the current pointer to the next parrent node
			Node* next = curr->pParentNode;
			curr->pParentNode = nullptr;
			curr = next;
		}
	}
	else
	{
		// End and start node are the same so just add the end position
		// on to the path instance
		currPathNode.v3Position = a_v3End;
		currPathNode.v3RotationPoint = a_v3End - glm::normalize(a_v3End - a_v3Start) * 3;
		a_rPath.AddPathNode(currPathNode);
	}
}

void PathfindSystem::GenerateTraversable(const glm::vec3& a_v3Min, const glm::vec3& a_v3Max, bool a_bValue)
{
	// Calculate the min and max of the vectors
	// This is needed to get the right order for the grid information
	glm::vec3 min = glm::min(a_v3Min, a_v3Max);
	glm::vec3 max = glm::max(a_v3Min, a_v3Max);

	// Convert the min and max value to grid values
	GridPosition gridmin = ConvertPositionToGrid(min);
	GridPosition gridmax = ConvertPositionToGrid(max);
	
	// Check if grid values are valid, if not than exit function
	if(!IsNodeGridValid(gridmin) || !IsNodeGridValid(gridmax))
		return;

	// Walk through nodes using the grid values and set the 
	// traversable value of the nodes to the Boolean parameter (a_bValue)
	for(unsigned z = gridmin.uGridZ ; z <= gridmax.uGridZ ; ++z)
	{
		for (unsigned x = gridmin.uGridX; x <= gridmax.uGridX; ++x)
		{
			unsigned index = ConvertGridToIndex(GridPosition{ x, z });
			m_vGrid[index].bIsFree = a_bValue;
		}
	}
}

int PathfindSystem::GetDistanceNodeToNode(const Node& start, const Node& end) const
{
	float distX = std::abs(start.v3Position.x - end.v3Position.x);
	float distZ = std::abs(start.v3Position.z - end.v3Position.z);

	return (distX > distZ) ? distZ * 14 + 10 * (distX - distZ) : distX * 14 + 10 * (distZ - distX);
}

Node& PathfindSystem::GetNodeFromIndex(unsigned a_uNodeIndex)
{
	if (!IsNodeIndexValid(a_uNodeIndex))
		a_uNodeIndex = CorrectIndexOnBounds(a_uNodeIndex);
		
	return m_vGrid[a_uNodeIndex];
}

Node& PathfindSystem::GetNodeFromPosition(glm::vec3 a_v3Position)
{
	if (!IsNodePositionValid(a_v3Position))
		a_v3Position = CorrectPositionOnBounds(a_v3Position);

	unsigned nodeindex = ConvertPositionToIndex(a_v3Position);
	return m_vGrid[nodeindex];
}

Node& PathfindSystem::GetNodeFromGrid(GridPosition a_gPosition)
{
	if (!IsNodeGridValid(a_gPosition))
		a_gPosition = CorrectGridOnBounds(a_gPosition);

	unsigned nodeindex = ConvertGridToIndex(a_gPosition);
	return m_vGrid[nodeindex];
}

unsigned PathfindSystem::ConvertPositionToIndex(const glm::vec3& a_v3Position) const
{
	return (((int)(a_v3Position.z * m_uInvNodeScalar)) * m_uNodeCountZ) + a_v3Position.x * m_uInvNodeScalar;
}

unsigned PathfindSystem::ConvertGridToIndex(const GridPosition& a_gPosition) const
{
	return (a_gPosition.uGridZ * m_uNodeCountZ + a_gPosition.uGridX);
}

glm::vec3 PathfindSystem::ConvertIndexToPosition(unsigned a_uNodeIndex) const
{
	GridPosition gridpos = ConvertIndexToGrid(a_uNodeIndex);

	return glm::vec3((gridpos.uGridX * m_uNodeScalar) + m_uInvNodeScalar, 0, (gridpos.uGridZ * m_uNodeScalar) + m_uInvNodeScalar);
}

glm::vec3 PathfindSystem::ConvertGridToPosition(const GridPosition& a_gPosition) const
{
	return glm::vec3((a_gPosition.uGridX * m_uNodeScalar) + m_uNodeOrigin, 0, (a_gPosition.uGridZ * m_uNodeScalar) + m_uNodeOrigin);
}

GridPosition PathfindSystem::ConvertIndexToGrid(unsigned a_uNodeIndex) const
{
	float index = a_uNodeIndex * m_fInvNodeCountX;

	unsigned gridZ = (unsigned)index;
	unsigned gridX = (unsigned)((index - gridZ) * m_uNodeCountX);

	return { gridX, gridZ };
}

GridPosition PathfindSystem::ConvertPositionToGrid(const glm::vec3& a_v3Position) const
{
	return { (unsigned)(a_v3Position.x * m_uInvNodeScalar), (unsigned)(a_v3Position.z * m_uInvNodeScalar) };
}

unsigned PathfindSystem::ConvertDirectionToIndex(const GridPosition& a_gPosition, NodeDirections direction) const
{
	GridPosition dirgrid = ConvertDirectionToGrid(a_gPosition, direction);
	return (IsNodeGridValid(dirgrid)) ? ConvertGridToIndex(dirgrid) : NODE_INDEX_RESERVED;
}

GridPosition PathfindSystem::ConvertDirectionToGrid(const GridPosition& a_gPosition, NodeDirections direction) const
{
	GridPosition gridpos = { NODE_INDEX_RESERVED, NODE_INDEX_RESERVED };

	switch (direction)
	{
	case (E_DIRECTION_UP):
		gridpos = { a_gPosition.uGridX, a_gPosition.uGridZ + 1 };
		break;
	case (E_DIRECTION_UP_RIGHT):
		gridpos = { a_gPosition.uGridX - 1, a_gPosition.uGridZ + 1 };
		break;
	case (E_DIRECTION_RIGHT):
		gridpos = { a_gPosition.uGridX - 1, a_gPosition.uGridZ };
		break;
	case (E_DIRECTION_DOWN_RIGHT):
		gridpos = { a_gPosition.uGridX - 1, a_gPosition.uGridZ - 1 };
		break;
	case (E_DIRECTION_DOWN):
		gridpos = { a_gPosition.uGridX, a_gPosition.uGridZ - 1 };
		break;
	case (E_DIRECTION_DOWN_LEFT):
		gridpos = { a_gPosition.uGridX + 1, a_gPosition.uGridZ - 1 };
		break;
	case (E_DIRECTION_LEFT):
		gridpos = { a_gPosition.uGridX + 1, a_gPosition.uGridZ };
		break;
	case (E_DIRECTION_UP_LEFT):
		gridpos = { a_gPosition.uGridX + 1, a_gPosition.uGridZ + 1 };
		break;
	default:
		gridpos = { NODE_INDEX_RESERVED, NODE_INDEX_RESERVED };
		break;
	}

	return gridpos;
}

unsigned PathfindSystem::CorrectIndexOnBounds(const unsigned& a_uIndex) const
{
	return Clamp(a_uIndex, 0, m_uNodeCountX * m_uNodeCountZ - 1);
}

glm::vec3 PathfindSystem::CorrectPositionOnBounds(const glm::vec3& a_v3Position) const
{
	return glm::vec3(glm::clamp(a_v3Position, glm::vec3(0, 0, 0), glm::vec3(m_uWorldWidth - m_fInvNodeCountX, 0, m_uWorldDepth - m_fInvNodeCountZ)));
}

GridPosition PathfindSystem::CorrectGridOnBounds(const GridPosition& a_gPosition) const
{
	return GridPosition{ Clamp(a_gPosition.uGridX, 0, m_uNodeCountX), Clamp(a_gPosition.uGridZ, 0, m_uNodeCountZ) };
}

bool PathfindSystem::IsNodeIndexValid(unsigned a_uNodeIndex) const
{
	return (a_uNodeIndex < m_vGrid.size() && a_uNodeIndex != NODE_INDEX_RESERVED);
}

bool PathfindSystem::IsNodePositionValid(const glm::vec3& a_v3Position) const
{
	return (a_v3Position.x <= m_uWorldWidth && a_v3Position.z <= m_uWorldDepth);
}

bool PathfindSystem::IsNodeGridValid(const GridPosition& a_gPosition) const
{
	return (a_gPosition.uGridX < m_uNodeCountX && a_gPosition.uGridZ < m_uNodeCountZ);
}


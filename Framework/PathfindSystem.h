#pragma once
#include "ComponentSystem.h"

#include "glm/vec3.hpp"
#include <vector>
#include "PathfindAgent.h"

// Enumerator for directions from a node
// there are 8 different directions
enum NodeDirections
{
	E_DIRECTION_UP,
	E_DIRECTION_UP_RIGHT,
	E_DIRECTION_RIGHT,
	E_DIRECTION_DOWN_RIGHT,
	E_DIRECTION_DOWN,
	E_DIRECTION_DOWN_LEFT,
	E_DIRECTION_LEFT,
	E_DIRECTION_UP_LEFT,
	E_DIRECTION_COUNT,
}
;

// Struct with information about a Node in general
// v3Position for the position with the world
// bIsFree defines if the node is accessible
// uaNeighbours is a array with indices to neighbors
// 
// Contains also information about pathfinding
// pParentNode for pathfinding algorithm, pointer for path
// fGCost defines the distance cost from the start
// fHCost defines the predicted cost to the end
// fFCost defines the total predicted cost ( fGCost and fHCost)
struct Node
{
	glm::vec3 v3Position;
	bool bIsFree;

	unsigned uaNeighbours[E_DIRECTION_COUNT];
	
	Node* pParentNode;
	float fGCost;
	float fHCost;
	float fFCost;
}
;

// Struct with information about the grid position
// uGridX defines the position of the node in rows
// uGridZ defines the position of the node in columns
struct GridPosition
{
	unsigned uGridX;
	unsigned uGridZ;
}
;

class Path;
class PathfindSystem : public ComponentSystem
{
public:
	PathfindSystem(unsigned int a_uiMapSize, unsigned int a_uiNodeScalar);

	void Render();

	bool FindPath(const glm::vec3& a_v3Start, const glm::vec3& a_v3End, Path& path);

	// Functions for generating traversable or none-traversable nodes
	void AddStaticNoneTraversable(const glm::vec3 a_v3Min, const glm::vec3 a_v3Max);
	void AddStaticTraversable(const glm::vec3 a_v3Min, const glm::vec3 a_v3Max);

	unsigned GetGridWidth() const;
	unsigned GetGridDepth() const;
protected:
	bool CanProcessComponent(Component* a_pComponent) override;
	void ProcessComponents() override;
	void OnComponentDestroyed(Component* a_pComponent) override;
	void OnComponentAdded(Component* a_pComponent) override;

private:
	// Function for creating path, requires a pointer to end node and reference to path
	void CreatePath(const glm::vec3& a_v3Start, const glm::vec3& a_v3End, Node& a_pStartNode, Node& a_pCurrentNode, Path& a_rPath);
	
	// Function for generating traversable nodes
	void GenerateTraversable(const glm::vec3& a_v3Min, const glm::vec3& a_v3Max, bool a_bValue);

	// Helper functions for calculating pathfinding
	int GetDistanceNodeToNode(const Node& start, const Node& end) const;

	// Helper functions for accessing nodes (assertions)
	Node& GetNodeFromIndex(unsigned a_uNodeIndex);
	Node& GetNodeFromPosition(glm::vec3 a_v3Position);
	Node& GetNodeFromGrid(GridPosition a_gPosition);

	// Helper functions for converting data
	unsigned ConvertPositionToIndex(const glm::vec3& a_v3Position) const;
	unsigned ConvertGridToIndex(const GridPosition& a_gPosition) const;

	glm::vec3 ConvertIndexToPosition(unsigned a_uNodeIndex) const;
	glm::vec3 ConvertGridToPosition(const GridPosition& a_gPosition) const;

	GridPosition ConvertIndexToGrid(unsigned a_uNodeIndex) const;
	GridPosition ConvertPositionToGrid(const glm::vec3& a_v3Position) const;

	// Helper functions for converting direction to other data (DIRECTIONS SPECIAL)
	unsigned ConvertDirectionToIndex(const GridPosition& a_gPosition, NodeDirections direction) const;
	GridPosition ConvertDirectionToGrid(const GridPosition& a_gPosition, NodeDirections direction) const;

	// Helper function for value correction when out of bounds
	unsigned CorrectIndexOnBounds(const unsigned& a_uIndex) const;
	glm::vec3 CorrectPositionOnBounds(const glm::vec3& a_v3Position) const;
	GridPosition CorrectGridOnBounds(const GridPosition& a_gPosition) const;
	
	// Helper functions for value checking (no assertions)
	bool IsNodeIndexValid(unsigned a_uNodeIndex) const;
	bool IsNodePositionValid(const glm::vec3& a_v3Position) const;
	bool IsNodeGridValid(const GridPosition& a_gPosition) const;
	
	/* Member variables */
	std::vector<Node> m_vGrid;
	std::vector<PathfindAgent*> m_vAgents;

	unsigned m_uWorldWidth;
	unsigned m_uWorldDepth;

	float m_uNodeScalar;
	float m_uNodeOrigin;
	float m_uInvNodeScalar;

	unsigned m_uNodeCountX;
	float m_fInvNodeCountX;

	unsigned m_uNodeCountZ;
	float m_fInvNodeCountZ;
};

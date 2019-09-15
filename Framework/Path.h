#pragma once

#include "glm/vec3.hpp"
#include <vector>

// Struct with information about path node
// pathnode is used within the path to give 
// information about the path to be walked
// v3Position is simply the position of the pathnode
struct PathNode
{
	glm::vec3 v3Position;
	glm::vec3 v3RotationPoint;
};

// Forward declaration
class PathfindingSystem;

// Class with information about a path
class Path
{
	// Pathfindsystem is a friend of this class
	friend class PathfindSystem;
public:
	Path();
	~Path();
	
	// Helper functions for passing on the path
	bool ProcessPath() const;
	bool ProcessPath(PathNode& pathnode) const;
	
	// Helper functions for getting a pathnode and position
	glm::vec3 GetCurrentPosition() const;
	glm::vec3 GetCurrentRotationPoint() const;
	
	unsigned GetCurrentPathNodeCount() const;
	const PathNode& GetCurrentPathNode() const;
	const std::vector<PathNode>& GetNodes() const { return m_vPath; }
	
	// Helper function for checking if path has reached end
	// and for processing last path node within path
	bool HasPathReachedEnd() const;
	bool IsProcessingLastPathNode() const;
private:
	// Helper functions for accesing and gaining information through the index 
	const glm::vec3& GetPathNodePosition(int a_uIndex) const;
	const glm::vec3& GetPathNodeRotationPoint(int a_uIndex) const;
	
	const PathNode& GetPathNodePathNode(int a_uIndex) const;
	
	// Functions for preparing the path and adding pathnodes
	void PreparePath(unsigned a_uPathNodeCount);
	void AddPathNode(PathNode& pathnode);
	
	/* Member variables */
	std::vector<PathNode> m_vPath;
	mutable int m_uCurrentNode;
};
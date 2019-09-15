#include "Path.h"

Path::Path()
	: m_vPath()
	, m_uCurrentNode(-1)
{
	// Defualt constructor setting the path value to none
}

Path::~Path()
{
	// Defualt de-constructor does nothing by default
}

bool Path::ProcessPath() const
{
	// Process the path using the other process path function
	PathNode pathnode; 
	return ProcessPath(pathnode);
}

bool Path::ProcessPath(PathNode& pathnode) const
{
	// Decrement the current path node index
	--m_uCurrentNode;
	
	// Check if there is a pathnode left by calling
	// the has reached end function which returns true or false
	if(!HasPathReachedEnd())
	{
		// Set the current path node and return true
		// if path hasn't reached end
		pathnode = GetCurrentPathNode();
		return true;
	}
	
	// Path has reached end, return false
	return false;
}

glm::vec3 Path::GetCurrentPosition() const
{
	return GetPathNodePosition(m_uCurrentNode);
}

glm::vec3 Path::GetCurrentRotationPoint() const
{
	return GetPathNodeRotationPoint(m_uCurrentNode);
}

unsigned Path::GetCurrentPathNodeCount() const
{
	return m_uCurrentNode;
}

const PathNode& Path::GetCurrentPathNode() const
{
	return GetPathNodePathNode(m_uCurrentNode);
}

bool Path::HasPathReachedEnd() const
{
	return (m_uCurrentNode < 0);
}

bool Path::IsProcessingLastPathNode() const
{
	return (m_uCurrentNode == 0);
}

const glm::vec3& Path::GetPathNodePosition(int a_uIndex) const
{
	return GetPathNodePathNode(a_uIndex).v3Position ;
}

const glm::vec3& Path::GetPathNodeRotationPoint(int a_uIndex) const
{
	return GetPathNodePathNode(a_uIndex).v3RotationPoint ;
}

const PathNode& Path::GetPathNodePathNode(int a_uIndex) const
{
	return m_vPath[a_uIndex];
}

void Path::PreparePath(unsigned a_uPathNodeCount)
{
	// Prepare path to reduce reallocating the vector
	// This is done by resizing the vector first to the path node count
	m_vPath.reserve(a_uPathNodeCount);
	m_uCurrentNode = a_uPathNodeCount - 1;
}

void Path::AddPathNode(PathNode& pathnode)
{
	// Add the pathnode to the vector and 
	// incrementing the current path node value by one 
	m_vPath.push_back(pathnode);
	
	if (m_uCurrentNode < (int)(m_vPath.size() - 1))
		++m_uCurrentNode;
}
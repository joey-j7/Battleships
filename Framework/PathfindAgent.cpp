#include "PathfindAgent.h"

PathfindAgent::PathfindAgent(GameObject* a_pObj) :
	Component(a_pObj)
{
	m_bHasDestination = false;
  SetType(E_Pathfinding);
}

void PathfindAgent::SetDestination(const glm::vec3& a_v3Target, std::function<void(bool, Path&)> a_callback)
{
	m_v3Destination = a_v3Target;
	m_pathCallback = a_callback;
	m_bHasDestination = true;
}

void PathfindAgent::OnPathReceived(bool a_bFoundPath, Path& a_path)
{
	m_pathCallback(a_bFoundPath, a_path);
	m_bHasDestination = false;
}
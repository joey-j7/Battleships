#pragma once
#include "Component.h"

class Path;
class PathfindAgent : public Component
{
public:
	friend class PathfindSystem;

	PathfindAgent(GameObject* a_pObj);

	void SetDestination(const glm::vec3& a_v3Target, std::function<void(bool, Path&)> a_callback);

	glm::vec3 GetDestination() const { return m_v3Destination; }
	bool HasDestination() const { return m_bHasDestination; }

private:
	void OnPathReceived(bool a_bFoundPath, Path& a_path);

	bool m_bHasDestination;
	glm::vec3 m_v3Destination;
	std::function<void(bool, Path&)> m_pathCallback;
};

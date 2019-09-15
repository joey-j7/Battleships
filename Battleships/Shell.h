#pragma once
#include "NetworkBehavior.h"

class Shell : public NetworkBehavior
{
public:
	Shell(GameObject* a_pObj, glm::vec3 a_v3Target);

	RakNet::RakString GetName() const override { return "Shell"; }

	void Start() override;
	void Update() override;

	void OnCollision(PhysicsBody* a_pOther) override;

private:
	float m_fArc;
	float m_fDistancedTraveled;
	float m_fDistance;
	float m_fSpeed;
	float m_fHeighOffset;

	glm::vec3 m_v3DirEuler;
	glm::vec3 m_v3Dir;
	glm::vec3 m_v3Target;
	glm::vec2 m_v2PrevSlope;
};

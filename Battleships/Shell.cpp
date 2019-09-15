#include "Shell.h"
#include "GameTime.h"
#include "BaseShip.h"
#include "PhysicsBody.h"
#include "NetworkIdentity.h"

Shell::Shell(GameObject* a_pObj, glm::vec3 a_v3Target) :
	NetworkBehavior(a_pObj)
{
	m_fArc = 4;
	m_fDistance = 0.f;
	m_fDistancedTraveled = 0.f;
	m_fHeighOffset = 0.f;
	m_fSpeed = 30.f;
	m_v3Target = a_v3Target;
	m_v2PrevSlope = glm::vec2();
}

void Shell::Start()
{
	m_v3Dir = glm::normalize(m_v3Target - m_pTransform->GetPosition());
	m_fDistance = glm::distance(m_v3Target, m_pTransform->GetPosition());
	m_fHeighOffset = m_pTransform->GetPosition().y;

	float yRot = glm::degrees(std::atan2(m_v3Dir.x, m_v3Dir.z));
	m_pTransform->SetRotation(glm::vec3(0.f, glm::radians(yRot), 0.f));
	m_v3DirEuler = glm::vec3(0.f, glm::radians(yRot), 0.f);

	GetIdentity()->SetSyncTransform(false);
}

void Shell::Update()
{
	m_fDistancedTraveled += m_fSpeed * GameTime::GetDeltaTime();
	glm::vec3 newPos = m_pTransform->GetPosition() + m_v3Dir * m_fSpeed * GameTime::GetDeltaTime();
	newPos.y = m_fHeighOffset + (-std::pow(m_fDistancedTraveled / m_fDistance, 2) + m_fDistancedTraveled / m_fDistance) * m_fDistance / m_fArc;

	const glm::vec2 slopeDiff = glm::vec2(m_fDistancedTraveled, newPos.y) - m_v2PrevSlope;
	const float derivative = slopeDiff.y / slopeDiff.x;
	m_v3DirEuler.x = -glm::atan(derivative / 1.f);

	m_v2PrevSlope = glm::vec2(m_fDistancedTraveled, newPos.y);
	m_pTransform->SetPosition(newPos);
	m_pTransform->SetRotation(m_v3DirEuler);
}

void Shell::OnCollision(PhysicsBody* a_pOther)
{
	BaseShip* pShip = a_pOther->GetComponent<BaseShip>();
	if (pShip && pShip->GetComponent<NetworkIdentity>()->GetOwnerGuid() == GetIdentity()->GetOwnerGuid()) return;

	DeSpawn();
}
#pragma once

#include "Path.h"
#include "PathfindAgent.h"
#include "NetworkBehavior.h"
#include "MeshRenderer.h"
#include <vector>

enum BaseShipCommands
{
	E_MOVE,
	E_ATTACK,
	E_HIT
};

class UnitController;
class HPBar;
class Shell;
class UnitStats;
class ParticleRenderer;

class BaseShip : public NetworkBehavior
{
public:
	BaseShip(GameObject* a_pObj);
	~BaseShip();
	
	RakNet::RakString GetName() const override { return "BaseShip"; }

	void Start() override;
	void Update() override;

	void GetShipsOnSight(const std::vector<BaseShip*>& a_vTargets, std::vector<BaseShip*>& a_vShipsOnSight);
	
	void OnCollision(PhysicsBody* a_pOther) override;
	void OnDeath();
	void OnEnemyDestroyed(Component* a_pEnemy);
	
	void MoveTowardsPoint(RakNet::BitStream& a_stream);
	void Attack(RakNet::BitStream& a_stream);
	void Hit(RakNet::BitStream& a_stream) const;

	bool IsDead() const { return m_bIsDead; }
	bool IsSelected() const { return m_bIsSelected; }

	void SetSelected(bool a_bIsSelected) { m_bIsSelected = a_bIsSelected; }

	const PhysicsBody* GetPhysicsBody() const { return m_pPhysicsBody; }
	MeshRenderer* GetMeshRenderer() const { return m_pMeshRenderer; }
	ParticleRenderer* GetParticleRenderer() const { return m_vParticleRenderers; }
	
	HPBar* GetHPBar() const
	{
		return m_pHPBar;
	}

private:
	bool HasPath() const;
	void FollowPath();
	void Fire(BaseShip* a_pShip);
	bool InFireRange(BaseShip* a_pShip) const;
	void RenderPath();

	void GenerateParticles();

	///@brief calculates shell accuracy based on distance to target and overal ship accuracy
	///@param a_v3Target The target position to hit
	float CalculateAccuracy(const glm::vec3& a_v3Target) const;

	///@brief calculates shell target by getting a random value inside a rectangle based on the ships size
	///@param a_pShip The ship to target
	///@param a_fAccuracy The accuracy of the shot, 1 = 100% and 0 = 50% chance to hit
	glm::vec3 CalculateShellTarget(BaseShip* a_pShip, float a_fAccuracy) const;

	float SinkAnimation() const;
	glm::quat m_fDeathRot;

	UnitController* m_pAttachedUnitController;
	ParticleRenderer* m_vParticleRenderers;
	HPBar* m_pHPBar;

	Path m_Path;
	PathfindAgent* m_pAgent;
	PhysicsBody* m_pPhysicsBody;
	MeshRenderer* m_pMeshRenderer;
	
	BaseShip* m_pTargetShip;
	UnitStats* m_pStats;
	
	glm::vec3 m_v3Velocity;
	float m_fMaxSpeed;
	float m_fAcceleration;
	
	float m_fAngleVelocity;
	float m_fMaxAngleSpeed;
	float m_fAngleAcceleration;
	
	float m_fViewDistance;
	
	float m_fRange;
	float m_fAccuracy;

	float m_fReloadTime;
	float m_fReloadTimer;

	bool m_bIsDead = false;
	bool m_bIsSelected;

#ifdef DEBUG
	const PhysicsBody* m_pTargetBody;
#endif // DEBUG
};

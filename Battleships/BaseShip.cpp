#include "BaseShip.h"

#include "GameTime.h"
#include "PhysicsBody.h"
#include "Debug.h"
#include "Utils.h"
#include "Shell.h"
#include "Model.h"
#include "NetworkIdentity.h"
#include "UnitController.h"
#include "PhysicsDebugDrawer.h"
#include "HPBar.h"
#include "UnitStats.h"
#include "NetworkSystem.h"
#include "ObjectSpawners.h"
#include "ParticleRenderer.h"
#include "IBO.h"
#include "PhysicsSystem.h"

BaseShip::BaseShip(GameObject* a_pObj)
	: NetworkBehavior(a_pObj)
{
	m_v3Velocity = glm::vec3(0, 0, 0);
	m_fMaxSpeed = 1.5f;
	m_fAcceleration = 1.5f;
	
	m_fAngleVelocity = 0.f;
	m_fMaxAngleSpeed = 0.2f;
	m_fAngleAcceleration = 0.03f;
	
	m_fViewDistance = 75.f;
	
	m_fRange = 70;
	m_fAccuracy = 0.7f;

	m_fReloadTime = 2;
	m_fReloadTimer = 0;

	m_pPhysicsBody = nullptr;
	m_pMeshRenderer = nullptr;
	m_pTargetShip = nullptr;
	m_pAgent = nullptr;
	m_bIsSelected = false;
	m_pAttachedUnitController = nullptr;

	m_pStats = new UnitStats(this);

	RegisterCommand(E_MOVE, std::bind(&BaseShip::MoveTowardsPoint, this, std::placeholders::_1));
	RegisterCommand(E_HIT, std::bind(&BaseShip::Hit, this, std::placeholders::_1));
	RegisterCommand(E_ATTACK, std::bind(&BaseShip::Attack, this, std::placeholders::_1));

	/* Add HP Bar HUD */
	m_pHPBar = new HPBar(a_pObj);

#if DEBUG
	m_pTargetBody = nullptr;		  
#endif // DEBUG
}

BaseShip::~BaseShip()
{
	delete m_pHPBar;
	delete m_pStats;
}

void BaseShip::Start()
{
	Requires<PathfindAgent>();
	Requires<PhysicsBody>();
	m_pAgent = GetComponent<PathfindAgent>();
	m_pPhysicsBody = GetComponent<PhysicsBody>();
	m_pMeshRenderer = GetComponent<MeshRenderer>();
	
	NetworkIdentity* pIdentity = GetComponent<NetworkIdentity>();
	GameObject* pPlayer = GameObject::FindGameObjectWithTag("Player" + std::string(pIdentity->GetOwnerGuid().ToString()));
	
	/* Create particle systems */
	//GenerateParticles();

	if (pPlayer)
	{
		m_pAttachedUnitController = pPlayer->GetComponent<UnitController>();
		m_pAttachedUnitController->AddShip(this);
	}
	else Debug::Log("Ship couldn't find player with tag: Player" + std::string(pIdentity->GetOwnerGuid().ToString()));
  
	m_pHPBar->SetAsEnemy(!GetIdentity()->HasAuthority());
}

void BaseShip::Update()
{
	m_pHPBar->Update(m_pStats->GetHP(), m_pStats->GetMaxHP());

	if (IsDead()) return;

	if (HasPath())
		FollowPath();

	m_fReloadTimer += GameTime::GetDeltaTime();
	
	if (m_pTargetShip && !m_pTargetShip->IsDead() && InFireRange(m_pTargetShip) && m_fReloadTimer > m_fReloadTime)
	{
		m_fReloadTimer = 0.f;
		if (m_pAttachedUnitController && !m_pAttachedUnitController->IsVisible(m_pTargetShip))
		{
			m_pTargetShip->Destroyed.UnSub(this);
			m_pTargetShip = nullptr;
		}
		else Fire(m_pTargetShip);
	}

	/*const float fSpeed = glm::length(m_v3Velocity);
	float fOpacity = (fSpeed < 0.01) ? 0 : fSpeed * 10.0f;

	fOpacity = glm::lerp(m_vParticleRenderers->GetOpacity(), fOpacity, 0.1f);
	m_vParticleRenderers->SetOpacity(fOpacity);*/

#ifdef DEBUG
	if (!m_pTargetBody || (m_pTargetShip && m_pTargetShip->IsDead())) return;

	static const float lineHeight = 2 / m_pTargetBody->GetObject()->GetTransform()->GetScale().x;
	glm::vec3 topLeft, topRight, botLeft, botRight;
	m_pTargetBody->GetModel()->GetBounds(botLeft, topRight);
	const glm::vec3 missMargin = (topRight - botLeft) * (1 - CalculateAccuracy(m_pTargetBody->GetObject()->GetTransform()->GetWorldPosition()));
	botLeft -= missMargin;
	topRight += missMargin;

	topLeft = glm::vec3(botLeft.x, lineHeight, topRight.z);
	topRight.y = lineHeight;
	botLeft.y = lineHeight;
	botRight = glm::vec3(topRight.x, lineHeight, botLeft.z);

	Transform* pTransform = m_pTargetBody->GetObject()->GetTransform();
	pTransform->TransformPoint(topLeft);
	pTransform->TransformPoint(topRight);
	pTransform->TransformPoint(botLeft);
	pTransform->TransformPoint(botRight);
	
	PhysicsDebugDrawer* pDrawer = PhysicsDebugDrawer::GetInstance();
	pDrawer->drawLine(*(btVector3*)&topLeft, *(btVector3*)&topRight, btVector3(0, 0, 1));
	pDrawer->drawLine(*(btVector3*)&topLeft, *(btVector3*)&botLeft, btVector3(0, 0, 1));
	pDrawer->drawLine(*(btVector3*)&botLeft, *(btVector3*)&botRight, btVector3(0, 0, 1));
	pDrawer->drawLine(*(btVector3*)&botRight, *(btVector3*)&topRight, btVector3(0, 0, 1));
#endif // DEBUG

}

void BaseShip::GetShipsOnSight(const std::vector<BaseShip*>& a_vTargets, std::vector<BaseShip*>& a_vShipsOnSight)
{
	PhysicsSystem* physics = GetSystem<PhysicsSystem>();
	glm::vec3 shippostion = m_pTransform->GetPosition();
	
	for (auto& i : a_vTargets)
	{
		glm::vec3 targetposition = i->GetObject()->GetTransform()->GetPosition();
		float distance = glm::length(targetposition - shippostion);
		if (distance <= m_fViewDistance)
		{
			GameObject* RayCastObject;
			if (shippostion != targetposition)
			{
				if (physics->RayCastWorld(shippostion, targetposition, RayCastObject))
				{
					if (RayCastObject == i->GetObject())
					{
						const auto foundIter = std::find(a_vShipsOnSight.begin(), a_vShipsOnSight.end(), i);
						if (foundIter == a_vShipsOnSight.end())
							a_vShipsOnSight.push_back(i);
					}
						
				}
			}
			else
			{
				const auto foundIter = std::find(a_vShipsOnSight.begin(), a_vShipsOnSight.end(), i);
				if (foundIter == a_vShipsOnSight.end())
					a_vShipsOnSight.push_back(i);
			}
		}
	}
}

void BaseShip::OnCollision(PhysicsBody* a_pOther)
{
	if (a_pOther->GetObject()->GetTag() == "WaterCollider") return;
	if (!GetClient()->IsHost()) return;

	Shell* pShell = a_pOther->GetComponent<Shell>();
	if (pShell)
	{
		if (pShell->GetComponent<NetworkIdentity>()->GetOwnerGuid() != GetIdentity()->GetOwnerGuid())
			RunCommand(E_HIT, GetIdentity()->GetId(), (unsigned int)Random::InRange(10, 20));
	}
}

void BaseShip::OnDeath()
{
	/* Ship starts sinking here */
	if (m_bIsDead) return;

	const glm::vec3 euler = glm::degrees(glm::eulerAngles(m_pTransform->GetRotation()));
	m_fDeathRot = m_pTransform->GetRotation() * glm::quat(glm::radians(glm::vec3(-30, 0, 0)));
	StartRoutine(std::bind(&BaseShip::SinkAnimation, this), "SinkAnimation");

	m_pHPBar->Hide();
	m_bIsDead = true;
}

void BaseShip::OnEnemyDestroyed(Component* a_pEnemy)
{
	m_pTargetShip = nullptr;
#ifdef DEBUG
	m_pTargetBody = nullptr;
#endif // DEBUG
}

void BaseShip::MoveTowardsPoint(RakNet::BitStream& a_stream)
{
	glm::vec3 v3Target;
	a_stream.Read(v3Target);
	m_pAgent->SetDestination(v3Target, [this](bool a_found, Path& a_path)
	{
		if (a_found)
			m_Path = a_path;
	});
}

void BaseShip::Attack(RakNet::BitStream& a_stream)
{
	if (!GetClient()->IsHost()) return;

	RakNet::RakString tag;
	a_stream.Read(tag);

	GameObject* pObj = GameObject::FindGameObjectWithTag(tag.C_String());
	BaseShip* pEnemy = pObj->GetComponent<BaseShip>();

	m_pTargetShip = pEnemy;
	m_pTargetShip->Destroyed.Sub(std::bind(&BaseShip::OnEnemyDestroyed, this, std::placeholders::_1), this);

#ifdef DEBUG
	m_pTargetBody = m_pTargetShip->GetPhysicsBody();
#endif // DEBUG
}

void BaseShip::Hit(RakNet::BitStream& a_stream) const
{
	unsigned int damage;
	a_stream.Read(damage);

	m_pStats->AddDamage(damage);
}

bool BaseShip::HasPath() const
{
	return !m_Path.HasPathReachedEnd();
}

void BaseShip::FollowPath()
{
	if (GetIdentity()->HasAuthority() && m_bIsSelected)
		RenderPath();
	
	glm::vec3 currentPosition = m_pTransform->GetPosition();
	glm::quat currentRotation = m_pTransform->GetRotation();
	glm::vec3 forward = m_pTransform->GetForward();
	
	glm::vec3 currNodePosition = m_Path.GetCurrentPosition();
	glm::vec3 currNodeRotationPoint = m_Path.GetCurrentRotationPoint();
	glm::vec3 directionNode = currNodeRotationPoint - currentPosition;
	glm::vec3 normalDirection = glm::normalize(directionNode);
	
	float speedScalar = m_fAcceleration * GameTime::GetDeltaTime();
	float speedRotation = 0.5f * GameTime::GetDeltaTime();
	float stoppingRadius = 5.f;
	float stoppingPower = 1 / stoppingRadius;
	
	float distNodeToRotationPoint = glm::length(currNodeRotationPoint - currNodePosition);
	float distToNodePosition = glm::length(directionNode);
	float angleDirection = std::atan2(normalDirection.x, normalDirection.z);
	
	if(angleDirection != std::atan2(forward.x, forward.z))
	{
		glm::quat targetRotation = glm::quat(glm::vec3(0, angleDirection, 0));
		m_fAngleVelocity += m_fAngleAcceleration;
		m_fAngleVelocity = std::max(0.f, std::min(m_fAngleVelocity, m_fMaxAngleSpeed));
		m_pTransform->SetRotation(glm::slerp(currentRotation, targetRotation, m_fAngleVelocity * GameTime::GetDeltaTime()));
	}
	
	forward = m_pTransform->GetForward();
	float rotationSpeedScalar = glm::dot(forward, normalDirection);
	if (rotationSpeedScalar < 0.3f)
		rotationSpeedScalar = 0.f;
	
	if ((m_Path.IsProcessingLastPathNode()) && (distToNodePosition < stoppingRadius))
		m_v3Velocity += ((forward * speedScalar * rotationSpeedScalar) * distToNodePosition * stoppingPower) - m_v3Velocity;
	else
		m_v3Velocity += (forward * speedScalar * rotationSpeedScalar) - m_v3Velocity;
	
	if (glm::length(m_v3Velocity) > m_fMaxSpeed)
		m_v3Velocity = glm::normalize(m_v3Velocity) * m_fMaxSpeed;
	
	m_pTransform->Translate(m_v3Velocity);
	
	if (m_Path.IsProcessingLastPathNode())
	{
		if (distToNodePosition < 1.f)
		{
			m_Path.ProcessPath();
			m_fAngleVelocity = 0;
		}
	}
	else
	{
		if (distToNodePosition < distNodeToRotationPoint)
			m_Path.ProcessPath();
	}
}

void BaseShip::Fire(BaseShip* a_pShip)
{
	const glm::vec3 enemyShipPos = a_pShip->GetObject()->GetTransform()->GetWorldPosition();
	const glm::vec3 shellTarget = CalculateShellTarget(a_pShip, CalculateAccuracy(enemyShipPos));

	RakNet::BitStream s;
	s.Write(shellTarget);
	GetNetworkSystem()->SpawnObject(NetworkObjectType::E_SHELL, m_pTransform->GetPosition(), glm::vec3(), glm::vec3(), &s, GetIdentity()->GetOwnerGuid());
}

bool BaseShip::InFireRange(BaseShip* a_pShip) const
{
	const glm::vec3 targetPos = a_pShip->GetObject()->GetTransform()->GetWorldPosition();
	return glm::distance(targetPos, m_pTransform->GetWorldPosition()) <= m_fRange; 
}

void BaseShip::RenderPath()
{
	for (unsigned int i = 0; i != m_Path.GetCurrentPathNodeCount() + 1; ++i)
	{
		// get a reference to the current node position
		const glm::vec3& NodePos = m_Path.GetNodes().at(i).v3Position;
		float lineheight = 2.f;

		btVector3 positionA;
		btVector3 positionB;

		if (i + 1 != m_Path.GetCurrentPathNodeCount() + 1)
		{
			const glm::vec3& SecondNodePos = m_Path.GetNodes().at(i + 1).v3Position;
			positionB = btVector3(NodePos.x, lineheight, NodePos.z);
			positionA = btVector3(SecondNodePos.x, lineheight, SecondNodePos.z);
		}
		else
		{
			const glm::vec3& ShipPos = m_pTransform->GetPosition();
			positionB = btVector3(NodePos.x, lineheight, NodePos.z);
			positionA = btVector3(ShipPos.x, lineheight, ShipPos.z);
		}

		PhysicsDebugDrawer::GetInstance()->drawLine(positionA, positionB, btVector3(1, 1, 0));
	}
}

void BaseShip::GenerateParticles()
{
	//	const glm::vec3 v3Extents = m_pMeshRenderer->GetMesh()->GetIBO()->GetExtents();
	//
	//	for (int f = 0; f < 2; ++f)
	//	{
	//		for (int side = 0; side < 2; ++side)
	//		{
	//			for (int b = 0; b < 2; ++b)
	//			{
	//				/* Rotation */
	//				const float rX = (f == 0 ? -45.0f : 45.0f);
	//				const float rY = (f == 0 ? 45.0f : 90.0f) + side * 45;
	//				const float rZ = 0.f;
	//
	//				GameObject* pParticles = GameObject::Create(
	//					m_pGameObject->GetTransform()->GetPosition() + glm::vec3(
	//						side == 0 ? v3Extents.x : -v3Extents.x,
	//						1.0f,
	//						v3Extents.z * (b == 0 ? 0.7f : -1)
	//					),
	//					glm::radians(glm::vec3(rX, rY, rZ)),
	//					glm::vec3(10, 10, 10)
	//				);
	//
	//				//pParticles->SetParent(m_pGameObject);
	//				pParticles->SetTag(std::string("Particle System F") + std::string(side == 0 ? "L" : "R") + to_string(f));
	//
	//				const ParticleRenderer::Data stData = {
	//					glm::vec3(1, 1, 1),
	//
	//					0.1f,
	//					(b == 0 ? 2.0f : 4.0f),
	//					(b == 0 ? 200.0f : 400.0f),
	//					(b == 0 ? 45.0f : 60.0f),
	//
	//					5,
	//					0.5f,
	//					f * 0.25f
	//				};
	//
	//				ParticleRenderer* pParticleRenderer = new ParticleRenderer(pParticles, stData);
	//				pParticleRenderer->SetOpacity(0.5f);
	//
	//				pParticles->AddComponent(pParticleRenderer);
	//			}
	//		}
	//	}

	const ParticleRenderer::Data stData = {
		glm::vec3(1, 1, 1),

		0.2f,
		-50.0f,
		5000.0f,
		0.0f,

		2,
		0.5f,
		0,
		glm::vec3(0, 1.0f, -m_pMeshRenderer->GetIBO()->GetExtents().z - 0.2f)
	};

	ParticleRenderer* pParticleRenderer = new ParticleRenderer(GetObject(), stData, "Assets/Game/Textures/Particles/water.png");

	GetObject()->AddComponent(pParticleRenderer);
	m_vParticleRenderers = pParticleRenderer;
}

float BaseShip::CalculateAccuracy(const glm::vec3& a_v3Target) const
{
	//Calculate chance the shell will hit, 0 = 50% 1 = 100%
	float shotAccuracy = (m_fRange - glm::distance(a_v3Target, m_pTransform->GetWorldPosition())) / m_fRange;
	shotAccuracy = std::max(0.5f, shotAccuracy);
	shotAccuracy *= m_fAccuracy * 1.8;
	shotAccuracy = glm::clamp(shotAccuracy, 0.f, 1.f);
	return shotAccuracy;
}

glm::vec3 BaseShip::CalculateShellTarget(BaseShip* a_pShip, float a_fAccuracy) const
{
	glm::vec3 min, max;
	a_pShip->GetPhysicsBody()->GetModel()->GetBounds(min, max);
	const glm::vec3 missMargin = (max - min) * (1 - a_fAccuracy);
	min -= missMargin;
	max += missMargin;

	const float x = Random::InRange(min.x, max.x);
	const float z = Random::InRange(min.z, max.z);
	glm::vec3 shellTarget(x, 0.f, z);
	a_pShip->GetObject()->GetTransform()->TransformPoint(shellTarget);

	return shellTarget;
}

float BaseShip::SinkAnimation() const
{
	const glm::quat currentRot = m_pTransform->GetRotation();

	if (glm::degrees(glm::angle(m_fDeathRot, currentRot)) < 1)
		m_pTransform->Translate(glm::vec3(0, -1 * GameTime::GetDeltaTime(), 0));
	else m_pTransform->LocalRotate(glm::radians(glm::vec3(-GameTime::GetDeltaTime() * 10, 0, 0)));

	if (m_pTransform->GetPosition().y < -10)
		return STOP_ROUTINE;
	return 0.f;
}

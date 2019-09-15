#include "PhysicsSystem.h"
#include "PhysicsBody.h"
#include "GameTime.h"
#include "PhysicsDebugDrawer.h"
#include "Application.h"
#include "Scene.h"

PhysicsSystem::PhysicsSystem()
{
	m_pBroadPhase = new btDbvtBroadphase();
	m_pCollisionConfiguration = new btDefaultCollisionConfiguration();
	m_pDispatcher = new btCollisionDispatcher(m_pCollisionConfiguration);
	m_pConstraintSolver = new btSequentialImpulseConstraintSolver();
	m_pWorld = new btDiscreteDynamicsWorld(m_pDispatcher, m_pBroadPhase, m_pConstraintSolver, m_pCollisionConfiguration);
	m_pWorld->setGravity(btVector3(0, -9.81f, 0));

	PhysicsDebugDrawer* debugDrawer = PhysicsDebugDrawer::GetInstance();
	m_pWorld->setDebugDrawer(debugDrawer);
	debugDrawer->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
}

PhysicsSystem::~PhysicsSystem()
{
	delete m_pBroadPhase;
	delete m_pCollisionConfiguration;
	delete m_pDispatcher;
	delete m_pConstraintSolver;
	delete m_pWorld;
}

bool PhysicsSystem::CanProcessComponent(Component* a_pComponent)
{
	return a_pComponent->GetType() == Component::E_Physics;
}

void PhysicsSystem::ProcessComponents()
{
	for (PhysicsBody* pBody : m_vComponents)
	{
		if (!pBody->IsStatic())
			pBody->Update();
	}

	//Update bullet physics
	m_pWorld->stepSimulation(GameTime::GetDeltaTime(), 10);

	for (PhysicsBody* pBody : m_vComponents)
		pBody->Sync();

	HandleCollisionEvents();

#ifdef DEBUG
	if (PhysicsDebugDrawer::GetInstance()->DoesDrawDebug())
		m_pWorld->debugDrawWorld();
#endif
}

void PhysicsSystem::OnComponentAdded(Component* a_pComponent)
{
	PhysicsBody* pBody = static_cast<PhysicsBody*>(a_pComponent);
	m_pWorld->addRigidBody(pBody->GetRigidBody());
	m_vComponents.push_back(pBody);
}

void PhysicsSystem::OnComponentDestroyed(Component* a_pComponent)
{
	const std::vector<PhysicsBody*>::iterator iter = std::find(m_vComponents.begin(), m_vComponents.end(), a_pComponent);
	if (iter != m_vComponents.end())
	{
		m_pWorld->removeRigidBody(static_cast<PhysicsBody*>(a_pComponent)->GetRigidBody());
		m_vComponents.erase(iter);
	}
}

bool PhysicsSystem::RayCastWorld(const glm::vec3 &a_v3Start, const glm::vec3 &a_v3End, GameObject * & a_pObject)
{
	glm::vec3 contactpoint;
	return RayCastWorld(a_v3Start, a_v3End, a_pObject, contactpoint);
}

bool PhysicsSystem::RayCastWorld(const btVector3 &a_v3Start, const btVector3 &a_v3End, GameObject * & a_pObject)
{
	btVector3 temp;
	return RayCastWorld(a_v3Start, a_v3End, a_pObject, temp);
}

bool PhysicsSystem::RayCastWorld(const glm::vec3 &a_v3Start, const glm::vec3 &a_v3End, GameObject * & a_pObject, glm::vec3 &a_v3ContactPoint)
{
	btVector3 startpoint = btVector3(a_v3Start.x, a_v3Start.y, a_v3Start.z);
	btVector3 endpoint = btVector3(a_v3End.x, a_v3End.y, a_v3End.z);
	btVector3 contactpoint = btVector3(a_v3ContactPoint.x, a_v3ContactPoint.y, a_v3ContactPoint.z);
	
	if (RayCastWorld(startpoint, endpoint, a_pObject, contactpoint))
	{
		a_v3ContactPoint = glm::vec3(contactpoint.x(), contactpoint.y(), contactpoint.z());
		return true;
	}
	
	return false;
}

bool PhysicsSystem::RayCastWorld(const btVector3 &a_v3Start, const btVector3 &a_v3End, GameObject * & a_pObject, btVector3 &a_v3ContactPoint)
{
	btCollisionWorld::ClosestRayResultCallback RayCallback(a_v3Start, a_v3End);
	
	m_pWorld->rayTest(a_v3Start, a_v3End, RayCallback);
	
#ifdef DEBUG
	if (PhysicsDebugDrawer::GetInstance()->DoesDrawDebug())
		PhysicsDebugDrawer::GetInstance()->drawLine(a_v3Start, a_v3End, btVector3(1, 0, 0));
#endif
	
	for (auto && it : m_vComponents)
	{
		if (it->GetRigidBody() == RayCallback.m_collisionObject)
		{
			a_pObject = it->GetObject();
			a_v3ContactPoint = RayCallback.m_hitPointWorld;
			return true;
		}
	}
	
	return false;
}

void PhysicsSystem::HandleCollisionEvents() const
{
	const int numManifolds = m_pWorld->getDispatcher()->getNumManifolds();
	for (int i = 0; i < numManifolds; i++)
	{
		btPersistentManifold* contactManifold = m_pWorld->getDispatcher()->getManifoldByIndexInternal(i);
		const btCollisionObject* obA = contactManifold->getBody0();
		const btCollisionObject* obB = contactManifold->getBody1();

		PhysicsBody* bodyA = (PhysicsBody*)obA->getUserPointer();
		PhysicsBody* bodyB = (PhysicsBody*)obB->getUserPointer();
		if (!bodyA || !bodyB) continue;

		const int numContacts = contactManifold->getNumContacts();
		for (int j = 0; j < numContacts; j++)
		{
			btManifoldPoint& pt = contactManifold->getContactPoint(j);
			if (pt.getDistance() < 0.f)
			{
				bodyA->OnCollision(bodyB);
				bodyB->OnCollision(bodyA);
				break;
			}
		}
	}
}

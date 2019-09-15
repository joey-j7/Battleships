#pragma once
#include "ComponentSystem.h"
#include <btBulletDynamicsCommon.h>

class PhysicsBody;
class PhysicsSystem: public ComponentSystem
{
public:
	PhysicsSystem();
	~PhysicsSystem();

	bool CanProcessComponent(Component* a_pComponent) override;
	void ProcessComponents() override;

	void OnComponentAdded(Component* a_pComponent) override;
	void OnComponentDestroyed(Component* a_pComponent) override;
	
	bool RayCastWorld(const glm::vec3 &a_v3Start, const glm::vec3 &a_v3End, GameObject * & a_pObject);
	bool RayCastWorld(const btVector3 &a_v3Start, const btVector3 &a_v3End, GameObject * & a_pObject);
	bool RayCastWorld(const glm::vec3 &a_v3Start, const glm::vec3 &a_v3End, GameObject * & a_pObject, glm::vec3 &a_v3ContactPoint);
	bool RayCastWorld(const btVector3 &a_v3Start, const btVector3 &a_v3End, GameObject* & a_pObject, btVector3 &a_v3ContactPoint);

	const std::vector<PhysicsBody*>& GetBodies() const { return m_vComponents; }

private:
	void HandleCollisionEvents() const;

	btBroadphaseInterface* m_pBroadPhase = nullptr;
	btDefaultCollisionConfiguration* m_pCollisionConfiguration = nullptr;
	btCollisionDispatcher* m_pDispatcher = nullptr;

	btSequentialImpulseConstraintSolver* m_pConstraintSolver = nullptr;
	btDiscreteDynamicsWorld* m_pWorld = nullptr;

	std::vector<PhysicsBody*> m_vComponents;
};

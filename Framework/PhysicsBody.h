#pragma once
#include "Component.h"
#include <btBulletDynamicsCommon.h>

class Model;

class PhysicsBody : public Component
{
public:
	PhysicsBody(GameObject* a_pObj, Model* a_pModel, btCollisionShape* a_pShape, float a_fMass = 1.f, bool a_bGravity = true, bool a_bTrigger = false);
	PhysicsBody(GameObject* a_pObj, btCollisionShape* a_pShape, float a_fMass = 1.f, bool a_bGravity = true, bool a_bTrigger = false);
	~PhysicsBody();

	Event<PhysicsBody*> OnCollision;

	btRigidBody* GetRigidBody() const { return m_pRigidBody; }
	btMotionState* GetMotionState() const { return m_pMotionState;}
	btCollisionShape* GetCollisionShape() const { return m_pShape; }
	Model* GetModel() const { return m_pModel; }

	void Start() override;

	void Update();
	void Sync() const;

	bool IsStatic() const { return m_fMass <= 0.f; }
	bool IsTrigger() const { return m_bIsTrigger; }

private:
	void Init(GameObject* a_pObj, btCollisionShape* a_pShape, float a_fMass = 1.f, bool a_bGravity = true, bool a_bTrigger = false);

	float m_fMass;
	bool m_bIsTrigger;

	glm::vec3 m_v3Offset = glm::vec3();
	
	Model* m_pModel = nullptr;
	btRigidBody* m_pRigidBody = nullptr;
	btMotionState* m_pMotionState = nullptr;
	btCollisionShape* m_pShape = nullptr;
};

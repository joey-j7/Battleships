#include "PhysicsBody.h"
#include "Transform.h"
#include "Utils.h"
#include "Model.h"

PhysicsBody::PhysicsBody(GameObject* a_pObj, Model* a_pModel, btCollisionShape* a_pShape, float a_fMass,
	bool a_bGravity, bool a_bTrigger) : Component(a_pObj)
{
	Init(a_pObj, a_pShape, a_fMass, a_bGravity, a_bTrigger);
	m_pModel = a_pModel;
}

PhysicsBody::PhysicsBody(GameObject* a_pObj, btCollisionShape* a_pShape, float a_fMass, bool a_bGravity, bool a_bTrigger)
	: Component(a_pObj)
{
	Init(a_pObj, a_pShape, a_fMass, a_bGravity, a_bTrigger);
}

PhysicsBody::~PhysicsBody()
{
	delete m_pRigidBody;
	delete m_pMotionState;
	delete m_pShape;
}

void PhysicsBody::Start()
{
	Transform* pTransform = GetObject()->GetTransform();
	const glm::mat4 m4Transform = pTransform->GetTransformMatrix();

	btTransform& tTransform = m_pRigidBody->getWorldTransform();
	const btScalar* mat = (btScalar*)&m4Transform;
	tTransform.setFromOpenGLMatrix(mat);
	m_pMotionState->setWorldTransform(tTransform);
}

void PhysicsBody::Update()
{
	Transform* pTransform = GetObject()->GetTransform();
	const glm::quat quat = pTransform->GetRotation();
	glm::vec3 position = pTransform->GetWorldPosition();

	if (m_pModel)
	{
		m_v3Offset = quat * m_pModel->GetOffset();
		position += m_v3Offset;
	}
	
	btTransform& tTransform = m_pRigidBody->getWorldTransform();
	const btQuaternion btQuat(quat.x, quat.y, quat.z, quat.w);
	tTransform.setRotation(btQuat);
	tTransform.setOrigin(btVector3(position.x, position.y, position.z));
	m_pMotionState->setWorldTransform(tTransform);
}

void PhysicsBody::Sync() const
{
	Transform* pTransform = GetObject()->GetTransform();

	float fMat[16];
	m_pRigidBody->getWorldTransform().getOpenGLMatrix(fMat);

	const glm::mat4 newMat = glm::make_mat4(fMat);
	pTransform->SetRotation(glm::quat(newMat));

	glm::vec3 position = Matrix4::GetTranslation(newMat);

	if (m_pModel)
	{
		position -= pTransform->GetRotation() * m_pModel->GetOffset();
	}

	pTransform->SetPosition(position);
}

void PhysicsBody::Init(GameObject* a_pObj, btCollisionShape* a_pShape, float a_fMass, bool a_bGravity, bool a_bTrigger)
{
	m_pShape = a_pShape;
	m_fMass = a_fMass;

	glm::vec3 v3Scale = a_pObj->GetTransform()->GetScale();
	m_pShape->setLocalScaling(btVector3(v3Scale.x, v3Scale.y, v3Scale.z));

	m_pMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));

	btVector3 v3Inertia(0, 0, 0);
	m_pShape->calculateLocalInertia(m_fMass, v3Inertia);
	const btRigidBody::btRigidBodyConstructionInfo rigidBodyCi(m_fMass, m_pMotionState, m_pShape, v3Inertia);
	m_pRigidBody = new btRigidBody(rigidBodyCi);
	m_pRigidBody->setUserPointer(this);
	m_pRigidBody->setSleepingThresholds(0.f, 0.f);

	if (!a_bGravity)
		m_pRigidBody->setLinearFactor(btVector3(1, 0, 1));

	if (a_bTrigger)
	{
		m_pRigidBody->setCollisionFlags(m_pRigidBody->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
		m_pRigidBody->setLinearFactor(btVector3(0, 0, 0));
		m_pRigidBody->setAngularFactor(btVector3(0, 0, 0));
		m_bIsTrigger = a_bTrigger;
	}

	SetType(E_Physics);
}

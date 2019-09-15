#include "Transform.h"
#include "Utils.h"
#include <glm/gtc/quaternion.hpp>
#include "Debug.h"
#include "GameTime.h"

Transform::Transform(GameObject* a_pParent) :
	Component(a_pParent)
{
	m_bIsDirty = true;
	m_bUpdated = true;

	m_m4TransformMatrix = glm::mat4(1.f);
	m_v3Scale = glm::vec3(1, 1, 1);
	m_v3LocalPosition = glm::vec3(0, 0, 0);
	m_qLocalRotation = glm::quat();
	m_fSequencePosition = 0.f;
}

const glm::mat4& Transform::GetTransformMatrix()
{
	if (m_bIsDirty)
	{	
		m_bUpdated = true;

		m_m4TransformMatrix = glm::translate(m_v3LocalPosition);
		m_m4TransformMatrix *= glm::toMat4(m_qLocalRotation);
		m_m4TransformMatrix *= glm::scale(m_v3Scale);

		if (GetObject()->GetParent())
		{
			Transform* pParentTransform = GetObject()->GetParent()->GetTransform();
			m_m4TransformMatrix = pParentTransform->GetTransformMatrix() * m_m4TransformMatrix;
		}
		
		m_bIsDirty = false;
	}

	return m_m4TransformMatrix;
}

glm::vec3 Transform::GetWorldPosition()
{
	const glm::mat4& m4WorldMat = GetTransformMatrix();
	return Matrix4::GetTranslation(m4WorldMat);
}

glm::quat Transform::GetWorldRotation()
{
	const glm::mat4& m4WorldMat = GetTransformMatrix();
	const glm::quat qRot = glm::quat_cast(m4WorldMat);
	return qRot;
}

glm::vec3 Transform::GetPosition() const
{
	return m_v3LocalPosition;
}

glm::quat Transform::GetRotation() const
{
	return m_qLocalRotation;
}

glm::vec3 Transform::GetScale() const
{
	return m_v3Scale;
}

glm::vec3 Transform::GetForward() const
{
	return glm::vec3(GetRotation() * glm::vec3(0, 0, 1));
}

glm::vec3 Transform::GetRight() const
{
	return glm::vec3(GetRotation() * glm::vec3(1, 0, 0));
}

glm::vec3 Transform::GetUp() const
{
	return glm::vec3(GetRotation() * glm::vec3(0, 1, 0));
}

void Transform::MoveTowards(glm::vec3 a_v3Target, float a_fMaxStep)
{
	glm::vec3 diff = a_v3Target - m_v3LocalPosition;
	float mag = glm::length(diff);
	if (mag <= a_fMaxStep || mag == 0.f)
	{
		m_v3LocalPosition = a_v3Target;
		SetDirty(true);
		return;
	}
	m_v3LocalPosition += diff / mag * a_fMaxStep;
	SetDirty(true);
}

void Transform::Translate(glm::vec3 a_v3Velocity)
{
	m_v3LocalPosition += a_v3Velocity;
	SetDirty(true);
}

void Transform::SetPosition(glm::vec3 a_v3Position)
{
	m_v3LocalPosition = a_v3Position;
	SetDirty(true);
}

void Transform::LocalRotate(glm::vec3 a_v3Rotation)
{
	m_qLocalRotation = m_qLocalRotation * glm::quat(a_v3Rotation);
	SetDirty(true);
}

void Transform::Rotate(glm::vec3 a_v3Rotation)
{
	m_qLocalRotation = glm::quat(a_v3Rotation) * m_qLocalRotation;
	SetDirty(true);
}

void Transform::SetRotation(glm::vec3 a_v3Rotation)
{
	m_qLocalRotation = glm::quat(a_v3Rotation);
	SetDirty(true);
}

void Transform::SetRotation(glm::quat a_qRot)
{
	m_qLocalRotation = a_qRot;
	SetDirty(true);
}

void Transform::SetScale(glm::vec3 a_v3Scale)
{
	m_v3Scale = a_v3Scale;
	SetDirty(true);
}

void Transform::SetMatrix(glm::mat4 a_matrix)
{
	m_bIsDirty = false;
	m_m4TransformMatrix = a_matrix;

	m_v3LocalPosition = Matrix4::GetTranslation(m_m4TransformMatrix);	
	m_v3Scale = Matrix4::GetScale(m_m4TransformMatrix);
	m_qLocalRotation = glm::quat(m_m4TransformMatrix);
}

void Transform::SetDirty(bool a_bIsdirty)
{
	m_bIsDirty = a_bIsdirty;

	for (GameObject* pObj : GetObject()->GetChildren())
		pObj->GetTransform()->SetDirty(a_bIsdirty);
}

void Transform::AddSequence(std::vector<Sequence> a_vSequences)
{
	m_vSequences.insert(m_vSequences.end(), a_vSequences.begin(), a_vSequences.end());
}

void Transform::PlaySequences()
{
	if (m_vSequences.empty())
		return;

	const Sequence csSequence = m_vSequences.front();
	const float fPositionNorm = m_fSequencePosition / csSequence.Duration;

	glm::vec3 v3Curr = csSequence.PosFrom + csSequence.PosDiff * fPositionNorm;
	SetPosition(v3Curr);
  
	v3Curr = csSequence.RotFrom + csSequence.RotDiff * fPositionNorm;
	SetRotation(glm::radians(v3Curr));

	if (m_fSequencePosition < csSequence.Duration) {
		m_fSequencePosition = std::min(m_fSequencePosition + GameTime::GetDeltaTime(), csSequence.Duration);
	}
	else {
		m_vSequences.erase(m_vSequences.begin());
		m_fSequencePosition = 0;
	}
}

void Transform::TransformPoint(glm::vec3& a_v3Point) const
{
	a_v3Point *= m_v3Scale;
	a_v3Point = m_qLocalRotation * a_v3Point;
	a_v3Point += m_v3LocalPosition;
}

glm::mat4 Transform::GetWorldMatrix()
{
	if (!GetObject()->GetParent())
		return GetTransformMatrix();
	return GetObject()->GetParent()->GetTransform()->GetWorldMatrix() * GetTransformMatrix();
}

#pragma once
#include "Component.h"
#include <glm/ext.hpp>

class Transform : public Component
{
public:
	struct Sequence {
		glm::vec3 PosFrom;
		glm::vec3 PosDiff;
	
		glm::vec3 RotFrom;
		glm::vec3 RotDiff;

		float Duration;
	};

	Transform(GameObject* a_pParent);

	const glm::mat4& GetTransformMatrix();

	glm::vec3 GetWorldPosition();
	glm::quat GetWorldRotation();

	glm::vec3 GetPosition() const;
	glm::quat GetRotation() const;
	glm::vec3 GetScale() const;

	glm::vec3 GetForward() const;
	glm::vec3 GetRight() const;
	glm::vec3 GetUp() const;

	void MoveTowards(glm::vec3 a_v3Target, float a_fMaxStep);

	void Translate(glm::vec3 a_v3Velocity);
	void SetPosition(glm::vec3 a_v3Position);

	void LocalRotate(glm::vec3 a_v3Rotation);
	void Rotate(glm::vec3 a_v3Rotation);
	void SetRotation(glm::vec3 a_v3Rotation);
	void SetRotation(glm::quat a_qRotation);

	void SetScale(glm::vec3 a_v3Scale);
	void SetMatrix(glm::mat4 a_m4Matrix);

	void SetDirty(bool a_bIsDirty);
	bool IsDirty() const { return m_bIsDirty; }

	void TransformPoint(glm::vec3& a_v3Point) const;

	void AddSequence(std::vector<Sequence> a_vSequences);
	void PlaySequences();
	int GetSequenceCount() const { return m_vSequences.size(); }
	bool HasSequences() const { return !m_vSequences.empty(); }
	
	bool IsUpdated() const
	{
		return m_bUpdated;
	}

	void Refresh()
	{
		m_bUpdated = false;
	}

private:
	glm::mat4 GetWorldMatrix();
  
	std::vector<Sequence> m_vSequences;
	float m_fSequencePosition;

	glm::mat4 m_m4TransformMatrix;

	glm::quat m_qLocalRotation;
	glm::vec3 m_v3LocalPosition;
	glm::vec3 m_v3Scale;

	bool m_bIsDirty;
	bool m_bUpdated;
};

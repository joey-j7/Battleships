#pragma once
#include "GameObject.h"
#include "Transform.h"
#include "PhysicsSystem.h"

class Mesh;

class Camera : public GameObject
{
public:
	struct Frustum
	{
		glm::vec4 Right;
		glm::vec4 Left;
		glm::vec4 Top;
		glm::vec4 Bottom;
		glm::vec4 Far;
		glm::vec4 Near;
	};

	Camera(float a_fFOV, float a_fRatio, float a_fNearPlane, float a_fFarPlane, PhysicsSystem* a_pPhysics);
	Camera(glm::mat4 a_m4ViewMatrix, glm::mat4 a_m4ProjectionMatrix)
	{
		m_m4ViewMatrix = a_m4ViewMatrix;
		m_m4ProjectionMatrix = a_m4ProjectionMatrix;
	}

	bool IsVisible(Transform* a_pTransform, Mesh* a_pMesh);
	bool IsInScreen(const glm::vec3& a_v3Position) const;

	bool IsInFrustum(const glm::vec3& a_v3Position, float a_fRadius);

	void SetViewMatrix(glm::mat4 a_m4ViewMat) { m_m4ViewMatrix = a_m4ViewMat; }

	void UpdateMatrix();

	const glm::mat4& GetViewMatrix()
	{
		UpdateMatrix();
		return m_m4ViewMatrix;
	}

	const glm::mat4& GetProjectionMatrix() const { return m_m4ProjectionMatrix; }

	const glm::mat4& GetProjectionViewMatrix()
	{
		UpdateMatrix();
		return m_m4ProjectionViewMatrix;
	}

	static void SetOrthoMatrix(const glm::mat4& a_m4OrthoMatrix) { m_m4OrthoMatrix = a_m4OrthoMatrix; };
	static const glm::mat4& GetOrthoMatrix() { return m_m4OrthoMatrix; }

	void SetScreenRatio(float a_fRatio);
		
	bool RayCastWorldOnMousePosition(GameObject*& a_pObject, glm::vec3& a_v3ContactPoint) const;
	bool RayCastWorld(glm::vec3 a_v3Start, glm::vec3 a_v3End, GameObject*& a_pObject, glm::vec3& a_v3ContactPoint) const;
	
	glm::vec3 ConvertScreenPositionToWorld(glm::vec2 a_v2Position) const;
	glm::vec2 ConvertWorldPositionToScreen(glm::vec3 a_v3Position) const;

	glm::vec3 GetForward() const;
	glm::vec3 GetRight() const;
	glm::vec3 GetUp() const;

protected:
	void GenerateFrustum();
	glm::vec4& GetPlane(unsigned int a_uiIndex);
  
	Frustum m_Frustum;

	glm::mat4 m_m4ViewMatrix;
	glm::mat4 m_m4ProjectionMatrix;
	glm::mat4 m_m4ProjectionViewMatrix;

	static glm::mat4 m_m4OrthoMatrix;

	float m_fFOV;
	float m_fScreenRatio;

	float m_fNearPlane;
	float m_fFarPlane;

private:
	PhysicsSystem* m_pPhysicsSystem;
};

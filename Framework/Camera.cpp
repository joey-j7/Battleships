#include "Camera.h"
#include "Transform.h"
#include "Utils.h"

#include "IBO.h"
#include "Mesh.h"

#include "Application.h"
#include "GameTime.h"

glm::mat4 Camera::m_m4OrthoMatrix = glm::mat4(1.0f);

Camera::Camera(float a_fFOV, float a_fScreenRatio, float a_fNearPlane, float a_fFarPlane, PhysicsSystem* a_pPhysics) : GameObject()
{
	m_fFOV = a_fFOV;
	m_fScreenRatio = a_fScreenRatio;
	m_fNearPlane = a_fNearPlane;
	m_fFarPlane = a_fFarPlane;
	m_pPhysicsSystem = a_pPhysics;

	m_m4ViewMatrix = glm::lookAt(glm::vec3(0, 0, 1), glm::vec3(), glm::vec3(0, 1, 0));
	m_m4ProjectionMatrix = glm::perspective(glm::radians(a_fFOV), a_fScreenRatio, a_fNearPlane, a_fFarPlane);
}

bool Camera::IsVisible(Transform* a_pTransform, Mesh* a_pMesh)
{
	IBO* pIBO = a_pMesh->GetIBO();

	const glm::vec3& v3Scale = a_pTransform->GetScale();
	const glm::vec3& v3Pos = a_pTransform->GetWorldPosition() + pIBO->GetOffset();

	const float fRadius = pIBO->GetRadius() * std::max(v3Scale.x, std::max(v3Scale.y, v3Scale.z));
	//PhysicsDebugDrawer::GetInstance()->drawSphere(*(btVector3*)&v3Pos, fRadius, btVector3(1, 1, 1));
	
	/* Sphere */
	return IsInFrustum(v3Pos, fRadius);
}

bool Camera::IsInScreen(const glm::vec3& a_v3Position) const
{
	const glm::vec2 v2WindowSize = Application::GetInstance()->GetGLContext()->GetGLWindow()->GetSize();
    
	return a_v3Position.x >= 0.0f &&
		a_v3Position.x <= v2WindowSize.x &&
		a_v3Position.y >= 0.0f &&
		a_v3Position.y <= v2WindowSize.y &&
		a_v3Position.z >= 0.0f
	;
}

void Camera::SetScreenRatio(float a_fRatio)
{
	m_m4ProjectionMatrix = glm::perspective(glm::radians(m_fFOV), a_fRatio, m_fNearPlane, m_fFarPlane);
}

bool Camera::RayCastWorldOnMousePosition(GameObject*& a_pObject, glm::vec3& a_v3ContactPoint) const
{
	glm::vec2 mousepos = Application::GetInstance()->GetInput()->GetMousePosition();
	glm::vec3 mousedirection = ConvertScreenPositionToWorld(mousepos);
		
	glm::vec3 startpos = GetTransform()->GetPosition() + mousedirection;
	glm::vec3 endpos = GetTransform()->GetPosition() + mousedirection * 1000;
	
	return RayCastWorld(startpos, endpos, a_pObject, a_v3ContactPoint);
}

bool Camera::RayCastWorld(glm::vec3 a_v3Start, glm::vec3 a_v3End, GameObject*& a_pObject, glm::vec3& a_v3ContactPoint) const
{
	btVector3 contactpoint;
	if (m_pPhysicsSystem->RayCastWorld(btVector3(a_v3Start.x, a_v3Start.y, a_v3Start.z), btVector3(a_v3End.x, a_v3End.y, a_v3End.z), a_pObject, contactpoint))
	{
		a_v3ContactPoint.x = contactpoint.getX();
		a_v3ContactPoint.y = contactpoint.getY();
		a_v3ContactPoint.z = contactpoint.getZ();
		
		return true;
	}
	
	return false;
}

glm::vec3 Camera::ConvertScreenPositionToWorld(glm::vec2 a_v2Position) const
{
	glm::vec2 windowsize = Application::GetInstance()->GetGLContext()->GetGLWindow()->GetSize();
  
	float devicecoordx = (2.f * a_v2Position.x) / windowsize.x - 1;
	float devicecoordy = (2.f * a_v2Position.y) / windowsize.y - 1;
    
	glm::vec4 clipCoords = glm::vec4(devicecoordx, -devicecoordy, -1.f, 1.f);
	
	glm::vec4 eyecoords = glm::inverse(m_m4ProjectionMatrix) * clipCoords;
	eyecoords.z = -1;
	eyecoords.w = 0;
    
	glm::vec4 worldcoords = glm::inverse(m_m4ViewMatrix) * eyecoords;
	glm::vec3 normalworldcoords = glm::normalize(glm::vec3(worldcoords.x, worldcoords.y, worldcoords.z));
    
	return normalworldcoords;
}

glm::vec2 Camera::ConvertWorldPositionToScreen(glm::vec3 a_v3Position) const
{
	const glm::vec4 clipCoords = m_m4ProjectionMatrix * (m_m4ViewMatrix * glm::vec4(a_v3Position, 1.0f));

	/* This position can't be converted to screen because it's behind the camera */
	if (clipCoords.z < 0)
		return glm::vec2(1.0f, 1.0f) * FLT_MIN;

	const glm::vec3 deviceCoords = clipCoords / clipCoords.w;
	const glm::vec2 windowsize = Application::GetInstance()->GetGLContext()->GetGLWindow()->GetSize();
    
	const int x = (int)glm::round((deviceCoords.x + 1.f) * 0.5f * windowsize.x);
	const int y = (int)glm::round((1.f - deviceCoords.y) * 0.5f * windowsize.y);
	
	return glm::vec2(x, y);
}

glm::vec3 Camera::GetForward() const
{
	return GetTransform()->GetRotation() * glm::vec3(0, 0, 1);
}

glm::vec3 Camera::GetRight() const
{
	return GetTransform()->GetRotation() * glm::vec3(1, 0, 0);
}

glm::vec3 Camera::GetUp() const
{
	return GetTransform()->GetRotation() * glm::vec3(0, 1, 0);
}

void Camera::GenerateFrustum()
{
	m_Frustum.Left.x = m_m4ProjectionViewMatrix[0][3] + m_m4ProjectionViewMatrix[0][0];
	m_Frustum.Left.y = m_m4ProjectionViewMatrix[1][3] + m_m4ProjectionViewMatrix[1][0];
	m_Frustum.Left.z = m_m4ProjectionViewMatrix[2][3] + m_m4ProjectionViewMatrix[2][0];
	m_Frustum.Left.w = m_m4ProjectionViewMatrix[3][3] + m_m4ProjectionViewMatrix[3][0];

	m_Frustum.Right.x = m_m4ProjectionViewMatrix[0][3] - m_m4ProjectionViewMatrix[0][0];
	m_Frustum.Right.y = m_m4ProjectionViewMatrix[1][3] - m_m4ProjectionViewMatrix[1][0];
	m_Frustum.Right.z = m_m4ProjectionViewMatrix[2][3] - m_m4ProjectionViewMatrix[2][0];
	m_Frustum.Right.w = m_m4ProjectionViewMatrix[3][3] - m_m4ProjectionViewMatrix[3][0];
  
	m_Frustum.Top.x = m_m4ProjectionViewMatrix[0][3] - m_m4ProjectionViewMatrix[0][1];
	m_Frustum.Top.y = m_m4ProjectionViewMatrix[1][3] - m_m4ProjectionViewMatrix[1][1];
	m_Frustum.Top.z = m_m4ProjectionViewMatrix[2][3] - m_m4ProjectionViewMatrix[2][1];
	m_Frustum.Top.w = m_m4ProjectionViewMatrix[3][3] - m_m4ProjectionViewMatrix[3][1];
  
	m_Frustum.Bottom.x = m_m4ProjectionViewMatrix[0][3] + m_m4ProjectionViewMatrix[0][1];
	m_Frustum.Bottom.y = m_m4ProjectionViewMatrix[1][3] + m_m4ProjectionViewMatrix[1][1];
	m_Frustum.Bottom.z = m_m4ProjectionViewMatrix[2][3] + m_m4ProjectionViewMatrix[2][1];
	m_Frustum.Bottom.w = m_m4ProjectionViewMatrix[3][3] + m_m4ProjectionViewMatrix[3][1];

	m_Frustum.Near.x = m_m4ProjectionViewMatrix[0][3] + m_m4ProjectionViewMatrix[0][2];
	m_Frustum.Near.y = m_m4ProjectionViewMatrix[1][3] + m_m4ProjectionViewMatrix[1][2];
	m_Frustum.Near.z = m_m4ProjectionViewMatrix[2][3] + m_m4ProjectionViewMatrix[2][2];
	m_Frustum.Near.w = m_m4ProjectionViewMatrix[3][3] + m_m4ProjectionViewMatrix[3][2];

	m_Frustum.Far.x = m_m4ProjectionViewMatrix[0][3] - m_m4ProjectionViewMatrix[0][2];
	m_Frustum.Far.y = m_m4ProjectionViewMatrix[1][3] - m_m4ProjectionViewMatrix[1][2];
	m_Frustum.Far.z = m_m4ProjectionViewMatrix[2][3] - m_m4ProjectionViewMatrix[2][2];
	m_Frustum.Far.w = m_m4ProjectionViewMatrix[3][3] - m_m4ProjectionViewMatrix[3][2];

	for (GLuint i = 0; i < 6; i++)
	{
		glm::vec4& p = GetPlane(i);

		float d = std::sqrt(
			p.x * p.x +
			p.y * p.y +
			p.z * p.z
		);

		p /= d;
	}
}

bool Camera::IsInFrustum(const glm::vec3& a_v3Position, float a_fRadius)
{
	for (GLuint p = 0; p < 6; p++)
	{
		const glm::vec4& v = GetPlane(p);

		if (v.x * a_v3Position.x + v.y * a_v3Position.y + v.z * a_v3Position.z + v.w <= -a_fRadius)
			return false;
	}
	
	return true;
}

glm::vec4& Camera::GetPlane(unsigned a_uiIndex)
{
	if (a_uiIndex == 0)
		return m_Frustum.Left;
	if (a_uiIndex == 1)
		return m_Frustum.Right;
	if (a_uiIndex == 2)
		return m_Frustum.Top;
	if (a_uiIndex == 3)
		return m_Frustum.Bottom;
	if (a_uiIndex == 4)
		return m_Frustum.Near;
	
	return m_Frustum.Far;
}

void Camera::UpdateMatrix()
{
	Transform* pTransform = GetTransform();
  
	if (pTransform->IsDirty() || pTransform->IsUpdated())
	{
		const glm::vec3 v3Translation = pTransform->GetWorldPosition();
		m_m4ViewMatrix = glm::lookAt(v3Translation, v3Translation + GetForward(), GetUp());
		m_m4ProjectionViewMatrix = m_m4ProjectionMatrix * m_m4ViewMatrix;

		GenerateFrustum();
	}
}

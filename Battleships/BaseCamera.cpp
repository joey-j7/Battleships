#include "BaseCamera.h"

#include "CameraController.h"
#include "GameTime.h"

#include "Camera.h"

#include <limits>
#include "Debug.h"

BaseCamera::BaseCamera(CameraController* cameracontroller)
	: m_pCameraController(cameracontroller)
{
	// Initialize default camera behaviour
	initialize();
}

BaseCamera::~BaseCamera()
{
	
}

void BaseCamera::UpdateCamera()
{
	UpdateCameraInput();
	UpdateCameraMovement();
}

void BaseCamera::Activate()
{
	m_v3TargetPosition = GetTransform()->GetWorldPosition();
	TargetPositionBoundingCheck();
	m_bHasPositionTarget = true;
	
	m_qfTargetRotation = m_qInitRotation;
	TargetRotationBoundingCheck();
	m_bHasRotationTarget = true;

  // Only lerp position on first frame
  if (!m_bInitialized)
  {
    GetTransform()->SetPosition(m_v3TargetPosition + glm::vec3(0, 100, 0));
    GetTransform()->SetRotation(m_qfTargetRotation);

    m_bHasRotationTarget = false;
    m_bInitialized = true;
  }
}

void BaseCamera::DeActivate()
{
	m_bHasPositionTarget = false;
	m_bHasRotationTarget = false;
}

void BaseCamera::UpdateCameraMovement()
{
  // Update position and rotation
	if(m_bHasPositionTarget)
	{
		glm::vec3 curposition = GetTransform()->GetWorldPosition();
		glm::vec3 newposition = glm::lerp(curposition, m_v3TargetPosition, glm::clamp(GetPositionSmoothing() * GameTime::GetDeltaTime(), 0.f, 1.f));
		
		GetTransform()->SetPosition(newposition);
		
		if (glm::distance(newposition, m_v3TargetPosition) < GameTime::GetDeltaTime())
			m_bHasPositionTarget = false;
	}
	
	if (m_bHasRotationTarget)
	{
		glm::quat currotation = GetTransform()->GetRotation();
		glm::quat newrotation = glm::lerp(currotation, m_qfTargetRotation, glm::clamp(GetRotationSmoothing() * GameTime::GetDeltaTime(), 0.f, 1.f));
		
		GetTransform()->SetRotation(newrotation);

		if (glm::angle(newrotation, m_qfTargetRotation) < GameTime::GetDeltaTime())
			m_bHasRotationTarget = false;
	}
}

void BaseCamera::TargetPositionBoundingCheck()
{
	// Position correction upon settings
	
	m_v3TargetPosition.x = glm::clamp(m_v3TargetPosition.x, (float) GetMinWidth(), (float) GetMaxWidth());
	m_v3TargetPosition.y = glm::clamp(m_v3TargetPosition.y, (float) GetMinHeight(), (float) GetMaxHeight());
	m_v3TargetPosition.z = glm::clamp(m_v3TargetPosition.z, (float) GetMinDepth(), (float) GetMaxDepth());
}

void BaseCamera::TargetRotationBoundingCheck()
{
	// Rotational correction upon settings
	
	glm::vec3 eulerAngles = glm::degrees(glm::eulerAngles(m_qfTargetRotation));
	
	if (	eulerAngles.x > GetMaxPitch() || eulerAngles.x < GetMinPitch() || 
			eulerAngles.y > GetMaxYaw() || eulerAngles.y < GetMinYaw() ||
			eulerAngles.z > GetMaxRoll() || eulerAngles.z < GetMinRoll())
	{
		eulerAngles.x = glm::clamp(eulerAngles.x, (float) GetMinPitch(), (float) GetMaxPitch());
		eulerAngles.y = glm::clamp(eulerAngles.y, (float) GetMinYaw(), (float) GetMaxYaw());
		eulerAngles.z = glm::clamp(eulerAngles.z, (float) GetMinRoll(), (float) GetMaxRoll());
	
		m_qfTargetRotation = glm::quat(glm::radians(eulerAngles));
	}
}

void BaseCamera::initialize()
{
	// Initialize default camera values for behaviour
	
	// Camera Lerp values for position and rotation 
	SetPositionSmoothing(5.f);
	SetRotationSmoothing(5.f);
	
	// Camera values for movement and input behaviour
	SetSensitivity(1.f);
	SetScrollSensitivity(2.f);
	SetCameraSpeed(30.f);
	
	// Camera values for locking of position
	
	SetMaxHeight(std::numeric_limits<int>::max());
	SetMinHeight(std::numeric_limits<int>::min());

	SetMaxWidth(std::numeric_limits<int>::max());
	SetMinWidth(std::numeric_limits<int>::min());
	
	SetMaxDepth(std::numeric_limits<int>::max());
	SetMinDepth(std::numeric_limits<int>::min());
	
	// Camera values for locking of rotation
	SetMaxPitch(180);
	SetMinPitch(-180);
	
	SetMaxYaw(180);
	SetMinYaw(-180);
	
	SetMaxRoll(180);
	SetMinRoll(-180);
}

const Camera* BaseCamera::GetCamera() const
{
	return m_pCameraController->GetCamera();
}

const InputManager* BaseCamera::GetInput() const
{
	return m_pCameraController->GetInput();
}

Transform* BaseCamera::GetTransform() const
{
	return m_pCameraController->GetTransform();
}

void BaseCamera::SetPosition(const glm::vec3& a_v3Pos)
{
	m_v3TargetPosition = a_v3Pos;
	TargetPositionBoundingCheck();
	m_bHasPositionTarget = true;
}

void BaseCamera::Translate(const glm::vec3& a_v3Translate)
{
	glm::vec3 translation = a_v3Translate * GameTime::GetDeltaTime() * (float) GetCameraSpeed();
	
	if (!m_bHasPositionTarget)
	{
		m_v3TargetPosition = GetTransform()->GetWorldPosition();
		m_v3TargetPosition += translation;
		TargetPositionBoundingCheck();
		
		m_bHasPositionTarget = true;
	}
	else
	{
		m_v3TargetPosition += translation;
		TargetPositionBoundingCheck();
	}
}

void BaseCamera::Rotate(const glm::vec3& a_v3Rotation)
{
	glm::vec3 rotate = a_v3Rotation * GameTime::GetDeltaTime() * GetSensitivity();
	
	if (!m_bHasRotationTarget)
	{
		m_qfTargetRotation = GetTransform()->GetRotation();
		
		glm::quat newRot = m_qfTargetRotation;
		newRot = glm::quat(glm::vec3(0, rotate.y, 0)) * newRot;
		newRot *= glm::quat(glm::vec3(rotate.x, 0, 0));
		m_qfTargetRotation = newRot;
		
		TargetRotationBoundingCheck();
		m_bHasRotationTarget = true;
	}
	else
	{
		glm::quat newRot = m_qfTargetRotation;
		newRot = glm::quat(glm::vec3(0, rotate.y, 0)) * newRot;
		newRot *= glm::quat(glm::vec3(rotate.x, 0, 0));
		m_qfTargetRotation = newRot;
		
		TargetRotationBoundingCheck();
	}
}

void BaseCamera::SetPositionSmoothing(float a_fPosLerpValue)
{
	m_fPosSmoothing = a_fPosLerpValue;
}

float BaseCamera::GetPositionSmoothing() const
{
	return m_fPosSmoothing;
}

void BaseCamera::SetRotationSmoothing(float a_fRotLerpValue)
{
	m_fRotSmoothing = a_fRotLerpValue;
}

float BaseCamera::GetRotationSmoothing() const
{
	return m_fRotSmoothing;
}

void BaseCamera::SetSensitivity(float a_fSensitivity)
{
	m_Sensitivity = a_fSensitivity;
}

float BaseCamera::GetSensitivity() const
{
	return m_Sensitivity;
}

void BaseCamera::SetScrollSensitivity(float a_fScrollSensitivity)
{
	m_ScrollSensitivity = a_fScrollSensitivity;
}

float BaseCamera::GetScrollSensitivity() const
{
	return m_ScrollSensitivity;
}
	
void BaseCamera::SetCameraSpeed(float a_fCameraspeed)
{
	m_CameraSpeed = a_fCameraspeed;
}

float BaseCamera::GetCameraSpeed() const
{
	return m_CameraSpeed;
}

void BaseCamera::SetMaxHeight(int a_iMaxHeight)
{
	m_MaxHeight = a_iMaxHeight;
}

int BaseCamera::GetMaxHeight() const
{
	return m_MaxHeight;
}

void BaseCamera::SetMinHeight(int a_iMinHeight)
{
	m_MinHeight = a_iMinHeight;
}

int BaseCamera::GetMinHeight() const
{
	return m_MinHeight;
}
	
void BaseCamera::SetMaxWidth(int a_iMaxWidth)
{
	m_MaxWidth = a_iMaxWidth;
}

int BaseCamera::GetMaxWidth() const
{
	return m_MaxWidth;
}

void BaseCamera::SetMinWidth(int a_iMinWidth)
{
	m_MinWidth = a_iMinWidth;
}

int BaseCamera::GetMinWidth() const
{
	return m_MinWidth;
}
	
void BaseCamera::SetMaxDepth(int a_iMaxDepth)
{
	m_MaxDepth = a_iMaxDepth;
}

int BaseCamera::GetMaxDepth() const
{
	return m_MaxDepth;
}

void BaseCamera::SetMinDepth(int a_iMinDepth)
{
	m_MinDepth = a_iMinDepth;
}

int BaseCamera::GetMinDepth() const
{
	return m_MinDepth;
}
	
void BaseCamera::SetMaxPitch(int a_iMaxPitch)
{
	m_MaxPitch = a_iMaxPitch;
}

int BaseCamera::GetMaxPitch() const
{
	return m_MaxPitch;
}

void BaseCamera::SetMinPitch(int a_iMinPitch)
{
	m_MinPitch = a_iMinPitch;
}

int BaseCamera::GetMinPitch() const
{
	return m_MinPitch;
}
	
void BaseCamera::SetMaxYaw(int a_iMaxYaw)
{
	m_MaxYaw = a_iMaxYaw;
}

int BaseCamera::GetMaxYaw() const
{
	return m_MaxYaw;
}

void BaseCamera::SetMinYaw(int a_iMinYaw)
{
	m_MinYaw = a_iMinYaw;
}

int BaseCamera::GetMinYaw() const
{
	return m_MinYaw;
}

void BaseCamera::SetMaxRoll(int a_iMaxRoll)
{
	m_MaxRoll = a_iMaxRoll;
}

int BaseCamera::GetMaxRoll() const
{
	return m_MaxRoll;
}

void BaseCamera::SetMinRoll(int a_iMinRoll)
{
	m_MinRoll = a_iMinRoll;
}

int BaseCamera::GetMinRoll() const
{
	return m_MinYaw;
}
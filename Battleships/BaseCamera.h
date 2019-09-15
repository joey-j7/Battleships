#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

class CameraController;
class InputManager;
class Transform;
class Camera;

class BaseCamera
{
public:
	BaseCamera(CameraController* cameracontroller);
	virtual ~BaseCamera();
	
	void UpdateCamera();
	
	void Activate();
	void DeActivate();

	void SetPosition(const glm::vec3& a_v3Pos);
	void Translate(const glm::vec3& a_v3Translate);
	void Rotate(const glm::vec3& a_v3Rotation);

	int GetMaxHeight() const;
	int GetMinHeight() const;

private:
	virtual void UpdateCameraInput() = 0;
	void UpdateCameraMovement();
	
	void TargetPositionBoundingCheck();
	void TargetRotationBoundingCheck();
	
	void initialize();
protected:
	glm::quat m_qInitRotation;
  bool m_bInitialized;

	const Camera* GetCamera() const;
	const InputManager* GetInput() const;
	Transform* GetTransform() const;
	
	void SetPositionSmoothing(float a_fPosLerpValue);
	float GetPositionSmoothing() const;
	void SetRotationSmoothing(float a_fRotLerpValue);
	float GetRotationSmoothing() const;
	
	void SetSensitivity(float a_fSensitivity);
	float GetSensitivity() const;
	
	void SetScrollSensitivity(float a_fScrollSensitivity);
	float GetScrollSensitivity() const;
	
	void SetCameraSpeed(float a_fCameraspeed);
	float GetCameraSpeed() const;

	void SetMaxHeight(int a_iMaxHeight);
	void SetMinHeight(int a_iMinHeight);

	void SetMaxWidth(int a_iMaxWidth);
	int GetMaxWidth() const;
	void SetMinWidth(int a_iMinWidth);
	int GetMinWidth() const;
	
	void SetMaxDepth(int a_iMaxDepth);
	int GetMaxDepth() const;
	void SetMinDepth(int a_iMinDepth);
	int GetMinDepth() const;
	
	void SetMaxPitch(int a_iMaxPitch);
	int GetMaxPitch() const;
	void SetMinPitch(int a_iMinPitch);
	int GetMinPitch() const;
	
	void SetMaxYaw(int a_iMaxYaw);
	int GetMaxYaw() const;
	void SetMinYaw(int a_iMinYaw);
	int GetMinYaw() const;
	
	void SetMaxRoll(int a_iMaxRoll);
	int GetMaxRoll() const;
	void SetMinRoll(int a_iMinRoll);
	int GetMinRoll() const;
private:
	CameraController* m_pCameraController;
	
	glm::vec3 m_v3TargetPosition;
	bool m_bHasPositionTarget;
	
	glm::quat m_qfTargetRotation;
	bool m_bHasRotationTarget;
	
	float m_fPosSmoothing;
	float m_fRotSmoothing;
	
	float m_Sensitivity;
	float m_ScrollSensitivity;
	float m_CameraSpeed;

	int m_MaxHeight;
	int m_MinHeight;
	
	int m_MaxWidth;
	int m_MinWidth;
	
	int m_MaxDepth;
	int m_MinDepth;
	
	int m_MaxPitch;
	int m_MinPitch;
	
	int m_MaxYaw;
	int m_MinYaw;
	
	int m_MaxRoll;
	int m_MinRoll;
};
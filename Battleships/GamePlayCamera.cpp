#include "GamePlayCamera.h"

#include "InputManager.h"
#include "Transform.h"
#include "GameTime.h"

#include "Camera.h"

GamePlayCamera::GamePlayCamera(CameraController* cameracontroller)
	: BaseCamera(cameracontroller)
{
	SetMaxHeight(100);
	SetMinHeight(20);

	SetMaxWidth(188);
	SetMinWidth(13);

	SetMaxDepth(178);
	SetMinDepth(-12);
	
	SetMaxPitch(60);
	SetMinPitch(60);
	SetMaxYaw(0);
	SetMinYaw(0);

	m_qInitRotation = glm::quat(glm::radians(glm::vec3(60, 0, 0)));
}

GamePlayCamera::~GamePlayCamera()
{
	
}

void GamePlayCamera::UpdateCameraInput()
{
  // Update keyboard input for camera behaviour

  #ifndef ANDROID
	if(GetInput()->KeyDown(Input::E_W))
		Translate(glm::vec3(0, 0, 1));

	if (GetInput()->KeyDown(Input::E_S))
		Translate(-glm::vec3(0, 0, 1));

	if (GetInput()->KeyDown(Input::E_D))
		Translate(-glm::vec3(1, 0, 0));

	if (GetInput()->KeyDown(Input::E_A))
		Translate(glm::vec3(1, 0, 0));
  
	// Update mouse input for camera behaviour
	
	int mousewheelvalue;
	if (GetInput()->MouseWheelScrolled(mousewheelvalue))
		Translate(mousewheelvalue * GetScrollSensitivity() * glm::vec3(0, 1, 0));
  #else
	if (GetInput()->MouseDown(Input::E_MOUSE_LEFT))
	{
		glm::vec2 v2Movement;

		/* Don't move camera on multitouch */
		if (!GetInput()->HasMultipleTouches())
		{
			const float fSpeedMultiplier = GetTransform()->GetPosition().y * 0.005f;
			v2Movement = GetInput()->GetMouseVelocityNorm() * 500 * fSpeedMultiplier;
		}

		const float v2PinchOffset = GetInput()->GetMousePinchOffsetNorm() * -150;
		Translate(glm::vec3(v2Movement.x, v2PinchOffset, -v2Movement.y));

		if (GetInput()->GetMousePressDistance() >= 100) {
			const glm::ivec2& v2ScreenSize = Application::GetInstance()->GetGLWindow()->GetSize();
			const glm::vec2 v2ScreenMargin = (glm::vec2)v2ScreenSize * 0.1f;
			const glm::ivec2& v2Position = GetInput()->GetMousePosition();

			if (glm::abs(v2Movement.x) <= 0.1f) {
				if (v2Position.x < v2ScreenMargin.x)
					Translate(-glm::vec3(1, 0, 0));
				else if (v2Position.x > v2ScreenSize.x - v2ScreenMargin.x)
					Translate(glm::vec3(1, 0, 0));
			}
		
			if (glm::abs(v2Movement.y) <= 0.1f) {
				if (v2Position.y < v2ScreenMargin.y)
					Translate(-glm::vec3(0, 0, 1));
				else if (v2Position.y > v2ScreenSize.y - v2ScreenMargin.y)
					Translate(glm::vec3(0, 0, 1));
			}
		}
	}	
  #endif
}
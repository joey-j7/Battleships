#include "FreeCamera.h"

#include "InputManager.h"
#include "Transform.h"
#include "GameTime.h"

#include "Camera.h"

FreeCamera::FreeCamera(CameraController* cameracontroller)
	: BaseCamera(cameracontroller)
{
	m_qInitRotation = glm::quat(glm::radians(glm::vec3(0, 0, 0)));
}

FreeCamera::~FreeCamera()
{
	
}

void FreeCamera::UpdateCameraInput()
{
	// Update keyboard input for camera behaviour
	
	if(GetInput()->KeyDown(Input::E_W))
		Translate(GetTransform()->GetForward());

	if (GetInput()->KeyDown(Input::E_S))
		Translate(-GetTransform()->GetForward());

	if (GetInput()->KeyDown(Input::E_D))
		Translate(-GetTransform()->GetRight());

	if (GetInput()->KeyDown(Input::E_A))
		Translate(GetTransform()->GetRight());
	
	if (GetInput()->KeyDown(Input::E_UP))
		Rotate(glm::vec3(-1, 0, 0));

	if (GetInput()->KeyDown(Input::E_DOWN))
		Rotate(glm::vec3(1, 0, 0));

	if (GetInput()->KeyDown(Input::E_RIGHT))
		Rotate(glm::vec3(0, -1, 0));

	if (GetInput()->KeyDown(Input::E_LEFT))
		Rotate(glm::vec3(0, 1, 0));

	if (GetInput()->KeyDown(Input::E_SPACE))
		Translate(GetTransform()->GetUp());

	if (GetInput()->KeyDown(Input::E_LEFT_CTRL))
		Translate(-GetTransform()->GetUp());
	
	
	// Update mouse input for camera behaviour
	
	int mousewheelvalue;
	if (GetInput()->MouseWheelScrolled(mousewheelvalue))
		Translate(mousewheelvalue * GetScrollSensitivity() * -GetTransform()->GetForward());
	
	if (GetInput()->MouseDown(Input::E_MOUSE_RIGHT))
	{
		glm::vec2 mousevelocity;
		if (GetInput()->MouseMoved(mousevelocity))
		{
//			GetTransform()->Rotate(glm::vec3(0, -mousevelocity.x * GameTime::GetDeltaTime() * GetSensitivity(), 0));
//			GetTransform()->LocalRotate(glm::vec3(-mousevelocity.y * GameTime::GetDeltaTime() * GetSensitivity(), 0, 0));
			Rotate(glm::vec3(-mousevelocity.y, -mousevelocity.x, 0));
		}
	}
	
	if (GetInput()->MouseDown(Input::E_MOUSE_LEFT))
	{
		glm::vec2 mousepos = GetInput()->GetMousePosition();
		glm::vec3 mouseworld = GetCamera()->ConvertScreenPositionToWorld(mousepos);
		
		std::cout << "MouseWorldPosition: " << mouseworld.x << "  " << mouseworld.y << "  " << mouseworld.z << std::endl;
	}
	
//	if(GetInput()->MouseDown(Input::E_MOUSE_RIGHT))
//	{
//		glm::ivec2 mousevelocity;
//		if (GetInput()->MouseMoved(mousevelocity))
//		{
//			glm::quat newRot = GetTransform()->GetRotation();
//			newRot = glm::quat(glm::vec3(0, -mousevelocity.x * GameTime::GetDeltaTime() * GetSensitivity(), 0)) * newRot;
//			newRot *= glm::quat(glm::vec3(-mousevelocity.y * GameTime::GetDeltaTime() * GetSensitivity(), 0, 0));
//			
//			GetTransform()->SetRotation(newRot);
//		}
//	}
}
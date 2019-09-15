#include "UnitCamera.h"

#include "InputManager.h"
#include "Transform.h"
#include "GameTime.h"
#include "Debug.h"

UnitCamera::UnitCamera(CameraController* cameracontroller)
	: BaseCamera(cameracontroller)
{
	m_qInitRotation = glm::quat(glm::radians(glm::vec3(0, 0, 0)));
}

UnitCamera::~UnitCamera()
{
	
}

void UnitCamera::UpdateCameraInput()
{
	// Update mouse input for camera behaviour
	if(GetInput()->MouseDown(Input::E_MOUSE_RIGHT))
	{
		glm::vec2 mousevelocity;
		if (GetInput()->MouseMoved(mousevelocity))
			Rotate(glm::vec3(0, -mousevelocity.x, 0));
	}
}
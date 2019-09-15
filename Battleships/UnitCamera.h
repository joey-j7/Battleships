#pragma once

#include "BaseCamera.h"
#include <glm/gtc/quaternion.hpp>

class UnitCamera : public BaseCamera
{
public:
	UnitCamera(CameraController* cameracontroller);
	virtual ~UnitCamera();
	
	virtual void UpdateCameraInput() override;
};

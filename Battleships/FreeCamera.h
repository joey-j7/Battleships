#pragma once

#include "BaseCamera.h"

class FreeCamera : public BaseCamera
{
public:
	FreeCamera(CameraController* cameracontroller);
	virtual ~FreeCamera();
	
	virtual void UpdateCameraInput() override;
};
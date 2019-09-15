#pragma once

#include "BaseCamera.h"

class GamePlayCamera : public BaseCamera
{
public:
	GamePlayCamera(CameraController* cameracontroller);
	virtual ~GamePlayCamera();
	
	virtual void UpdateCameraInput() override;
};
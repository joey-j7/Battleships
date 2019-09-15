#pragma once

#include "BehaviorScript.h"
#include "CameraIdentifiers.h"

class Camera;
class BaseCamera;

class CameraController : public BehaviorScript
{
public:
	CameraController(GameObject* a_pObj);
	~CameraController();

	void Start() override;
	void Update() override;

	const Camera* GetCamera() const;
	const InputManager* GetInput() const;
	Transform* GetTransform() const;
	BaseCamera* GetActiveModus() const;
	
private:
	void UpdateCameraInput();
	void UpdateCameraModus();
	
	void SetCameraModus(unsigned a_uModusIndex);
	unsigned GetCameraModus() const;
	
private:
	GameObject* m_pPlayer = nullptr;
	bool m_isAttachedToPlayer;
	
	BaseCamera* m_CameraModus[CameraModus::COUNT];
	unsigned m_uCurrCameraModus;
};

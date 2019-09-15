#include "CameraController.h"
#include "GameTime.h"

#include "GamePlayCamera.h"
#include "FreeCamera.h"
#include "UnitCamera.h"

#include "Application.h"
#include "SceneManager.h"
#include "Scene.h"

CameraController::CameraController(GameObject* a_pObj)
	: BehaviorScript(a_pObj)
{
	m_isAttachedToPlayer = false;
	
	// Creation and initialization of camera's
	m_CameraModus[CameraModus::CAMERA_GAMEPLAY] = new GamePlayCamera(this);
	m_CameraModus[CameraModus::CAMERA_UNIT] = new UnitCamera(this);
	m_CameraModus[CameraModus::CAMERA_FREE] = new FreeCamera(this);
}

CameraController::~CameraController()
{
	// Delete all the camera's to clean up memory
	unsigned cameracount = static_cast<unsigned>(CameraModus::COUNT);
	
	for (unsigned int i = 0; i != cameracount; ++i)
		delete m_CameraModus[i];
}

void CameraController::Start()
{
	m_pPlayer = GameObject::FindGameObjectWithTag("Player");
  
	//	m_pTransform->SetPosition(glm::vec3(0, 20, -10));
	//	m_pTransform->SetRotation(glm::vec3(0, 0, 0));
	
	m_pTransform->SetPosition(glm::vec3(55, 50, 35));
	m_pTransform->SetRotation(glm::vec3(0, 0, 0));
	
	// Set current camera modus 
	m_uCurrCameraModus = CameraModus::CAMERA_GAMEPLAY;
	m_CameraModus[GetCameraModus()]->Activate();
}

void CameraController::Update()
{
	// Update local camera controller
	UpdateCameraInput();
	
	// Update current camera modus
	UpdateCameraModus();
}

const Camera* CameraController::GetCamera() const
{
	return Application::GetInstance()->GetSceneManager()->GetActiveScene()->GetCamera();
}

BaseCamera* CameraController::GetActiveModus() const
{
	return m_CameraModus[GetCameraModus()];
}

const InputManager* CameraController::GetInput() const
{
	return m_pInput;
}

Transform* CameraController::GetTransform() const
{
	return m_pTransform;
}

void CameraController::UpdateCameraInput()
{
#ifdef DEBUG
	if (m_pInput->KeyDown(Input::E_F8))
		SetCameraModus(CameraModus::CAMERA_GAMEPLAY);
	
	if (m_pInput->KeyDown(Input::E_F9))
		SetCameraModus(CameraModus::CAMERA_UNIT);
	
	if (m_pInput->KeyDown(Input::E_F10))
		SetCameraModus(CameraModus::CAMERA_FREE);
#endif
}

void CameraController::UpdateCameraModus()
{
	m_CameraModus[GetCameraModus()]->UpdateCamera();
}

void CameraController::SetCameraModus(unsigned a_uModusIndex)
{
	if (a_uModusIndex < CameraModus::COUNT)
	{
		// Deactivate old camera modus
		m_CameraModus[GetCameraModus()]->DeActivate();
		
		// Set and activate new camera modus
		m_uCurrCameraModus = a_uModusIndex;
		m_CameraModus[GetCameraModus()]->Activate();
	}
}

unsigned CameraController::GetCameraModus() const
{
	return m_uCurrCameraModus;
}
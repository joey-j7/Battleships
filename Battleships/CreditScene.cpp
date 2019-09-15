#include "CreditScene.h"
#include "GameObject.h"
#include "SpriteRenderer.h"
#include "SceneManager.h"
#include "MainMenuScene.h"
#include "Transform.h"
#include "Camera.h"
#include "LoadScreen.h"
#include "AudioSource.h"
#include "../Framework/InputManager.h"

CreditScene::CreditScene(Application* a_pApp) :
	Scene(a_pApp)
{
	m_pSceneLoader = nullptr;
}

void CreditScene::Init()
{
	//Pre load images
	m_preLoadedTextures[0] = Application::GetInstance()->GetLoadInterface()->LoadTexture("Assets/Game/Textures/UI/Loading Screens/splash.jpg");
	m_preLoadedTextures[1] = Application::GetInstance()->GetLoadInterface()->LoadTexture("Assets/Game/Textures/UI/Loading Screens/black.png");

	/* Add Music */
	AudioFile* pBGM = m_pApplication->GetLoadInterface()->LoadAudio("Assets/Game/Sound/BGM/Credits.wav");
	AudioSource* pAudioSource = new AudioSource(m_pMainCamera, pBGM);
	m_pMainCamera->AddComponent(pAudioSource);

	GameObject* pObj = GameObject::Create();
	pObj->SetTag("Credits");
	m_pCreditsRenderer = new SpriteRenderer(pObj, "Assets/Game/Textures/Credits/credits.png", false);
	pObj->AddComponent(m_pCreditsRenderer);

	Transform* pTransform = pObj->GetTransform();
	pTransform->SetPosition(glm::vec3(1920 * 0.5f, 1080 * 0.5f, 0.0f));
	pTransform->SetScale(glm::vec3(1920, 1080, 1.0f));

	/* Enable Cursor */
	Application::GetInstance()->GetCursor()->SetActive(true);

	pAudioSource->Play();
}

void CreditScene::OnUpdate(float a_deltaTime)
{
	if (m_pSceneLoader != nullptr) return;

	InputManager* pInput = Application::GetInstance()->GetInput();
	const Input::KeyState* keyStates = pInput->GetKeyStates();
	for (int i = 0; i < 512; ++i)
	{
		if (keyStates[i] == Input::E_KEY_DOWN || keyStates[i] == Input::E_KEY_PRESSED || pInput->MouseDown(Input::E_MOUSE_LEFT))
		{
			//Create load screen object
			m_pSceneLoader = GameObject::Create();
			m_pSceneLoader->AddComponent<LoadScreen>(new MainMenuScene(Application::GetInstance()), m_pMainCamera->GetComponent<AudioSource>());
			return;
		}
	}	
}

void CreditScene::DeInit()
{
	m_pCreditsRenderer->SetEnabled(false);
	Application::GetInstance()->GetLoadInterface()->DeleteTexture(m_preLoadedTextures[0]->GetPath());
	Application::GetInstance()->GetLoadInterface()->DeleteTexture(m_preLoadedTextures[1]->GetPath());
}

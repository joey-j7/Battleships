#include "MainMenuScene.h"
#include "MainMenuWindow.h"
#include "LobbyWindow.h"
#include "NetworkSystem.h"
#include "LobbyManager.h"
#include "AudioSource.h"
#include "CreditScene.h"
#include "LoadScreen.h"
#include "DebugScene.h"
#include "WaterRenderer.h"
#include "GameTime.h"
#include "GameProperties.h"
#include "RenderSystem.h"
#include "imgui_internal.h"

MainMenuScene::MainMenuScene(Application* a_pApplication) :
	Scene(a_pApplication)
{
	m_pMainMenu = nullptr;
	m_pLobby = nullptr;
	m_pCamOrbitPivot = nullptr;
}

std::vector<ComponentSystem*> MainMenuScene::GetSystems()
{
	return std::vector<ComponentSystem*> { new NetworkSystem(30.f) };
}

void MainMenuScene::Init()
{
	//Pre load images
	m_preLoadedTextures[0] = Application::GetInstance()->GetLoadInterface()->LoadTexture("Assets/Game/Textures/UI/Loading Screens/splash.jpg");
	m_preLoadedTextures[1] = Application::GetInstance()->GetLoadInterface()->LoadTexture("Assets/Game/Textures/UI/Loading Screens/black.png");

	/* Enable Cursor */
	Application::GetInstance()->GetCursor()->SetActive(true);

	SetupMenu();
	SetupWorld();
	
	GameObject* pLogo = GameObject::Create();
	pLogo->SetTag("Logo");

	SpriteRenderer* pLogoRenderer = new SpriteRenderer(pLogo, "Assets/Game/Textures/MainMenu/Logo_HQ.png", false);
	m_pLogoTransform = pLogo->GetTransform();
	SetLogoPosition();
	
	pLogo->AddComponent(pLogoRenderer);
	
	RenderSystem::LightObject->GetTransform()->SetPosition(glm::vec3(0, 10, -10));
}

void MainMenuScene::DeInit()
{
	/* Disable Cursor */
	Application::GetInstance()->GetCursor()->SetActive(false);

	m_pLobby->Destroy();
	m_pMainMenu->Destroy();

	Application::GetInstance()->GetLoadInterface()->DeleteTexture(m_preLoadedTextures[0]->GetPath());
	Application::GetInstance()->GetLoadInterface()->DeleteTexture(m_preLoadedTextures[1]->GetPath());
}

void MainMenuScene::Pause()
{
	m_pLobby->SetActive(false);
	m_pMainMenu->SetActive(false);
}

void MainMenuScene::Resume()
{
	m_pLobby->SetActive(true);
	m_pMainMenu->SetActive(true);
}

void MainMenuScene::OnUpdate(float a_deltaTime)
{
	for (int i = 0; i < 3; ++i)
	{
		const float rot = sin(GameTime::GetTime() + i) * 0.03 + 0.03;
		m_pShipTransform[i]->Translate(glm::vec3(-10 * a_deltaTime, 0, 0));
		m_pShipTransform[i]->SetRotation(glm::vec3(glm::radians(0.f), glm::radians(-90.f), -rot));
	}

	m_pCamOrbitPivot->GetTransform()->Rotate(glm::vec3(0, a_deltaTime * 0.1, 0));
	m_pMainCamera->GetTransform()->Rotate(glm::vec3(0, a_deltaTime * 0.1, 0));

	SetLogoPosition();
}

void MainMenuScene::SetupMenu()
{
	m_pMainMenu = new MainMenuWindow();
	
	m_pSettings = new GameProperties();
	m_pSettings->SetActive(false);

	m_pLobby = new LobbyWindow();
	m_pLobby->SetActive(false);

	GameObject* pLobbyManager = GameObject::Create();
	pLobbyManager->AddComponent<LobbyManager>(m_pMainMenu, m_pLobby);
	pLobbyManager->AddComponent<NetworkIdentity>(NetworkIdentity::GenerateId());

	/* Play Audio */
	AudioFile* pBGM = m_pApplication->GetLoadInterface()->LoadAudio("Assets/Game/Sound/BGM/Main_Theme.wav");
	AudioSource* pAudioSource = new AudioSource(m_pMainCamera, pBGM);
	m_pMainCamera->AddComponent(pAudioSource);

	/* Play Music */
	pAudioSource->Play();

	m_pMainMenu->OnCreditsPressed.Sub([this, pAudioSource]()
	{
		//Create load screen object
		GameObject* pLoadObject = GameObject::Create();
		pLoadObject->AddComponent<LoadScreen>(new CreditScene(Application::GetInstance()), pAudioSource);
	}, this);

	m_pMainMenu->OnDebugPressed.Sub([pAudioSource]()
	{
		//Create load screen object
		GameObject* pLoadObject = GameObject::Create();
		pLoadObject->AddComponent<LoadScreen>(new DebugScene(Application::GetInstance()), pAudioSource);
	}, this);

	m_pMainMenu->OnSettingsPressed.Sub([this]()
	{
		m_pSettings->SetActive(true);
		m_pMainMenu->SetActive(false);
		m_pSettings->SetPrevious(m_pMainMenu);
	}, this);
}

void MainMenuScene::SetupWorld()
{
	m_pMainCamera->SetTag("Camera");
	m_pMainCamera->GetTransform()->SetPosition(glm::vec3(-4, 2, 0));
	m_pMainCamera->GetTransform()->SetRotation(glm::vec3(glm::radians(30.f), 0, 0));

	/* Set Water Surface */
	GameObject* pWaterPlane = GameObject::Create(glm::vec3(), glm::vec3(), glm::vec3(400, 1, 400));
	pWaterPlane->SetTag("Water Surface");
	pWaterPlane->AddComponent<WaterRenderer>();

	//Create ships
	for (int i = 0; i < 3; ++i)
	{
		std::pair<GameObject*, Model*> pShipPair = Model::CreateObject("Assets/Game/Models/Battleships/model.obj");
		if (Application::GetInstance()->IsHighQuality()) {
			pShipPair = Model::CreateObject(
				"Assets/Game/Models/Battleships/model.obj",
				"Assets/Engine/Shaders/Default/PBR.vsh",
				"Assets/Engine/Shaders/Default/PBR_ship.fsh"
			);
		}
		else
		{
			pShipPair = Model::CreateObject(
				"Assets/Game/Models/Battleships/model.obj"
			);
		}

		m_pShipTransform[i] = pShipPair.first->GetTransform();
		m_pShipTransform[i]->SetScale(glm::vec3(10, 10, 10));
		m_pShipTransform[i]->SetPosition(glm::vec3(0, 0.3, i * 20));
		m_pShipTransform[i]->SetRotation(glm::vec3(0, glm::radians(-90.f), 0));
	}
	m_pShipTransform[1]->Translate(glm::vec3(-15, 0, 0));

	m_pCamOrbitPivot = GameObject::Create();
	m_pCamOrbitPivot->SetParent(m_pShipTransform[1]->GetObject());

	m_pMainCamera->SetParent(m_pCamOrbitPivot);
}

void MainMenuScene::SetLogoPosition() const
{
	const int iLogoSize = glm::min(1024, 1920 - 20);

	m_pLogoTransform->SetPosition(glm::vec3(1920 * 0.5f, iLogoSize * 0.5f + 100, 0.0f));
	m_pLogoTransform->SetScale(glm::vec3(iLogoSize, iLogoSize, 1.0f));
}

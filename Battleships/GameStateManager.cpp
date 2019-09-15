#include "GameStateManager.h"
#include "PathfindSystem.h"
#include "ObjectSpawners.h"
#include "Camera.h"
#include "BaseCamera.h"
#include "UnitController.h"
#include "Debug.h"
#include "SceneManager.h"
#include "MainMenuScene.h"
#include "SpriteRenderer.h"
#include "AudioSource.h"
#include "PacketEnums.h"
#include "SoundFader.h"
#include "LoadScreen.h"
#include "GameTime.h"

GameStateManager::GameStateManager(GameObject* a_pObj) :
	NetworkBehavior(a_pObj)
{
	m_fEndTimer = 10.f;
	m_uiShipsSpawned = 0;
	m_currentState = E_GAME_LOADING;
	m_pEnemyUnitController = nullptr;
	m_pPlayerUnitController = nullptr;

	RegisterCommand(E_BEGIN_GAME, std::bind(&GameStateManager::BeginGame, this));
	RegisterCommand(E_END_GAME, std::bind(&GameStateManager::EndGame, this, std::placeholders::_1));

	GetObject()->AddComponent<SoundFader>();
}

GameStateManager::~GameStateManager()
{
	NetworkSystem* pNetworkSystem = GetSystem<NetworkSystem>();
	pNetworkSystem->OnObjectSpawn.UnSub(this);
}

void GameStateManager::Update()
{
	if (m_pEnemyUnitController && m_pPlayerUnitController && GetClient()->IsHost() && m_currentState != E_GAME_ENDED)
	{
		if (m_pEnemyUnitController->GetNumAliveShips() <= 0)
		{
			m_currentState = E_GAME_ENDED;
			RunCommandAll(E_END_GAME, m_pPlayerUnitController->GetIdentity()->GetOwnerGuid());
		}
		else if (m_pPlayerUnitController->GetNumAliveShips() <= 0)
		{
			m_currentState = E_GAME_ENDED;
			RunCommandAll(E_END_GAME, m_pEnemyUnitController->GetIdentity()->GetOwnerGuid());
		}
	}

	if (m_currentState == E_GAME_ENDED)
	{
		m_fEndTimer -= GameTime::GetDeltaTime();
		if (m_fEndTimer <= 0.f)
		{
			InputManager* pInput = Application::GetInstance()->GetInput();
			const Input::KeyState* keyStates = pInput->GetKeyStates();
			for (int i = 0; i < 512; ++i)
			{
				if (keyStates[i] == Input::E_KEY_DOWN || keyStates[i] == Input::E_KEY_PRESSED || pInput->MouseDown(Input::E_MOUSE_LEFT))
				{
					ExitLobby();
					m_currentState = E_GAME_LEAVING;
					break;
				}
			}
		}
	}
}

void GameStateManager::Start()
{
	m_pCameraController = GameObject::FindGameObjectWithTag("Camera")->GetComponent<CameraController>();
	if(!GetClient()->IsHost())
	{
		RunCommandAll(E_BEGIN_GAME);
	}
}

void GameStateManager::BeginGame()
{
	NetworkSystem* pNetworkSystem = GetSystem<NetworkSystem>();
	pNetworkSystem->OnObjectSpawn.Sub(std::bind(&GameStateManager::OnObjectSpawned, this, std::placeholders::_1, std::placeholders::_2), this);
	PathfindSystem* pPathfindSystem = GetSystem<PathfindSystem>();
		
	pNetworkSystem->SpawnObject(E_PLAYER, nullptr, GetClient()->GetGuid());

	const float shipSpacing = 10;
	const float startPosHeightOffset = 10;
	float yRot = 180;
	glm::vec3 startPos = glm::vec3((float)pPathfindSystem->GetGridWidth() / 2.f - (MAX_SHIPS * shipSpacing / 2.f), 0.3f, (float)pPathfindSystem->GetGridDepth() - startPosHeightOffset);
	if (GetClient()->IsHost())
	{
		startPos.z = startPosHeightOffset;
		yRot = 0;
	}
	
	for (int i = 0; i < MAX_SHIPS; ++i)
		pNetworkSystem->SpawnObject(E_SHIP, startPos + glm::vec3(i * shipSpacing, 0, 0), glm::radians(glm::vec3(0, yRot, 0)), glm::vec3(5, 5, 5), nullptr, GetClient()->GetGuid());
	
	//Set camera position above units
	BaseCamera* pModus = m_pCameraController->GetActiveModus();
	startPos.y += pModus->GetMaxHeight() - 10;
	startPos.z -= 20;
	startPos.x += MAX_SHIPS * shipSpacing / 2.f;
	pModus->SetPosition(startPos);

	m_pCameraController->SetEnabled(true);
}

void GameStateManager::EndGame(RakNet::BitStream& a_stream)
{
	m_currentState = E_GAME_ENDED;

	/* Disable Cursor */
	Application::GetInstance()->GetCursor()->SetActive(false);

	//Destroy player unit controller so the player can't control ships anymore
	m_pPlayerUnitController->Destroy();
	m_pPlayerUnitController = nullptr;

	RakNet::RakNetGUID winnerGuid;
	a_stream.Read(winnerGuid);

	std::string resultSpritePath;
	std::string resultMusicPath;
	if (winnerGuid == GetClient()->GetGuid())
	{
		resultSpritePath = "Assets/Game/Textures/UI/Result Screens/win.jpg";
		resultMusicPath = "Assets/Game/Sound/BGM/Results_Win.wav";
		Debug::Log("Winner");
	}
	else
	{
		resultSpritePath = "Assets/Game/Textures/UI/Result Screens/lose.jpg";
		resultMusicPath = "Assets/Game/Sound/BGM/Results_Loss.wav";
		Debug::Log("Loser");
	}

	//Show end screen with delay
	Invoke([resultSpritePath, resultMusicPath, this]()
	{
		GetObject()->AddComponent<SpriteRenderer>(resultSpritePath, false);

		//Disable camera controls
		Camera* pCam = GetCamera();
		pCam->GetComponent<CameraController>()->SetEnabled(false);

		//Set battle result sprite renderer
		Transform* pTransform = GetObject()->GetTransform();
		pTransform->SetPosition(glm::vec3(1920 * 0.5f, 1080 * 0.5f, 0.0f));
		pTransform->SetScale(glm::vec3(1920, 1080, 1.0f));

		AudioFile* pBGM = Application::GetInstance()->GetLoadInterface()->LoadAudio(resultMusicPath);
		GetComponent<SoundFader>()->Transition(pCam->GetComponent<AudioSource>(), pBGM, 2.f);
	}, 5.f);

	//Return to main menu and disconnect to peers after showing end screen for a bit
	Invoke([this]()
	{
		ExitLobby();
	}, 55.f);
}

void GameStateManager::ExitLobby() const
{
	GetClient()->Disconnect();

	//Create load screen object
	GameObject* pLoadObject = GameObject::Create();
	pLoadObject->AddComponent<LoadScreen>(new MainMenuScene(Application::GetInstance()), GetCamera()->GetComponent<AudioSource>());
}

void GameStateManager::OnObjectSpawned(SpawnEvent& a_spawnEvent, GameObject* a_pObj)
{
	if (a_spawnEvent.SpawnId == NetworkObjectType::E_SHIP && m_currentState != E_GAME_INPROGRESS)
	{
		++m_uiShipsSpawned;
		if (m_uiShipsSpawned >= MAX_SHIPS * 2)
		{
			const NetworkUser& peer = GetClient()->GetConnections()[0];
			GameObject* pPlayer = GameObject::FindGameObjectWithTag("Player" + std::string(GetClient()->GetGuid().ToString()));
			GameObject* pEnemyPlayer = GameObject::FindGameObjectWithTag("Player" + std::string(peer.Guid.ToString()));
			if (pPlayer && pEnemyPlayer)
			{
				m_pPlayerUnitController = pPlayer->GetComponent<UnitController>();
				m_pEnemyUnitController = pEnemyPlayer->GetComponent<UnitController>();
				m_pPlayerUnitController->AddEnemyShips(m_pEnemyUnitController->GetManagedShips());
				m_pEnemyUnitController->AddEnemyShips(m_pPlayerUnitController->GetManagedShips());

				m_currentState = E_GAME_INPROGRESS;
				if (GetClient()->IsHost())
					GetClient()->SendCommand(PacketEnum::E_GAME_STARTED);

				Debug::Log("Game in progress");
				Debug::Log("Enemy has " + to_string(m_pEnemyUnitController->GetManagedShips().size()) + " ships");
				Debug::Log("Player has " + to_string(m_pPlayerUnitController->GetManagedShips().size()) + " ships");
			}
		}
	}
}

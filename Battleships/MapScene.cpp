#include "MapScene.h"

#include "Application.h"
#include "PathfindSystem.h"
#include "ObjectSpawners.h"
#include "NetworkSystem.h"
#include "MeshRenderer.h"
#include <bullet/include/LinearMath/btVector3.h>
#include "HQWaterRenderer.h"
#include "WaterRenderer.h"
#include "CameraController.h"
#include "Camera.h"
#include "PhysicsBody.h"
#include "PacketEnums.h"
#include "SceneManager.h"
#include "RenderSystem.h"
#include "AudioSource.h"

MapScene::MapScene(Application* a_pApplication) :
	Scene(a_pApplication)
{
	m_bGameStartInit = false;
	m_bGameInProgress = false;
	m_pClient = nullptr;
	m_fPollTimer = 0.f;
}

void MapScene::Init()
{
	//Pre load images
	m_preLoadedTextures[0] = Application::GetInstance()->GetLoadInterface()->LoadTexture("Assets/Game/Textures/UI/Loading Screens/splash.jpg");
	m_preLoadedTextures[1] = Application::GetInstance()->GetLoadInterface()->LoadTexture("Assets/Game/Textures/UI/Loading Screens/black.png");

	//Create load screen object
	GameObject* pLoadObject = GameObject::Create();
	m_pLoadScreen = new LoadScreen(pLoadObject);
	pLoadObject->AddComponent(m_pLoadScreen);

	m_pClient = Application::GetInstance()->GetClient();
	m_pClient->OnPacketReceived.Sub(std::bind(&MapScene::OnPacketReceived, this, std::placeholders::_1), this);
  
	/* Add Music */
	AudioFile* pBGM = m_pApplication->GetLoadInterface()->LoadAudio("Assets/Game/Sound/BGM/Battle_loop.wav");
	AudioSource* pAudioSource = new AudioSource(m_pMainCamera, pBGM);
	m_pMainCamera->AddComponent(pAudioSource);
  
	pAudioSource->Play();
}

void MapScene::DeInit()
{
	m_pClient->OnPacketReceived.UnSub(this);
	Application::GetInstance()->GetLoadInterface()->DeleteTexture(m_preLoadedTextures[0]->GetPath());
	Application::GetInstance()->GetLoadInterface()->DeleteTexture(m_preLoadedTextures[1]->GetPath());
}

std::vector<ComponentSystem*> MapScene::GetSystems()
{
	return std::vector<ComponentSystem*> { new NetworkSystem(30.f, GetSpawnFuncs()), new PathfindSystem(200, 10) };
}

void MapScene::OnUpdate(float a_deltaTime)
{
	if (m_bGameInProgress)
		m_pLoadScreen->Disable();

	if (!m_pClient->IsHost() && !m_bGameStartInit)
	{
		m_fPollTimer += a_deltaTime;
		if (m_fPollTimer >= 1.f)
		{
			m_fPollTimer = 0.f;
			m_pClient->SendCommand(E_PLAYER_LOADED);
		}
	}

	glm::vec3 v3CameraPos = m_pMainCamera->GetTransform()->GetPosition();

	v3CameraPos.y = 1;
	v3CameraPos.z += 200;
	RenderSystem::LightObject->GetTransform()->SetPosition(v3CameraPos);
}

void MapScene::OnPacketReceived(RakNet::Packet* a_pPacket)
{
	switch (a_pPacket->data[0])
	{
	case PacketEnum::E_PLAYER_LOADED:
		m_pClient->SendCommand(E_START_GAME);
		break;
	case PacketEnum::E_START_GAME:
	{
		if (m_bGameStartInit) return;
		m_bGameStartInit = true;
		InitGame();
		break;
	}
	case PacketEnum::E_GAME_STARTED:
	{
		m_pClient->SendMessage(E_GAME_STARTED);
		m_bGameInProgress = true;

		/* Enable Cursor */
		Application::GetInstance()->GetCursor()->SetActive(true);
		break;
	}
	default: ;
	}
}

void MapScene::InitGame()
{
	CreateWorld();

	/* Set camera controller */
	CameraController* pController = new CameraController(m_pMainCamera);
	pController->SetEnabled(false);
	m_pMainCamera->AddComponent(pController);
	m_pMainCamera->SetTag("Camera");

	if (m_pClient->IsHost())
	{
		NetworkSystem* pNetworkSystem = GetSystem<NetworkSystem>();
		pNetworkSystem->SpawnObject(E_GAME_MANAGER);
	}
}

void MapScene::CreateWorld()
{
	/* Get pathfind system for creating static objects */
	PathfindSystem* pathfindsystem = GetSystem<PathfindSystem>();

	/* Set Water Surface */
	GameObject* pWaterPlane = GameObject::Create(glm::vec3(), glm::vec3(), glm::vec3(265, 1, 265));
	pWaterPlane->SetTag("Water Surface");

	if (Application::GetInstance()->IsHighQuality())
	{
#ifdef WINDOWS
		pWaterPlane = GameObject::Create(glm::vec3(100, 0, 100), glm::vec3(), glm::vec3(0.23, 0.15, 0.23));
		pWaterPlane->AddComponent<HQWaterRenderer>();

		const std::pair<GameObject*, Model*> pBorder = Model::CreateObject(
			"Assets/Game/Models/Border/border.obj",
			"Assets/Engine/Shaders/Default/PBR.vsh",
			"Assets/Engine/Shaders/Default/PBR_rock.fsh");

		pBorder.first->GetTransform()->SetPosition(glm::vec3(-3.9f, 104.0f, 256.3f));
		pBorder.first->GetTransform()->SetScale(glm::vec3(1.2f, 1.0f, 1.2f));
#endif // WINDOWS
	}
	else
	{
		pWaterPlane = GameObject::Create(glm::vec3(), glm::vec3(), glm::vec3(265, 1, 265));
		pWaterPlane->AddComponent<WaterRenderer>();
	}

	/* Set Pathfinding Collision Shape*/
	GameObject* pPathfindingCollisionShape = GameObject::Create(glm::vec3(pathfindsystem->GetGridWidth() * 0.5, -1, pathfindsystem->GetGridDepth() * 0.5), glm::vec3(), glm::vec3(1, 1, 1));
	btCollisionShape* pPathCollisionShape = new btBoxShape(btVector3(pathfindsystem->GetGridWidth(), 1, pathfindsystem->GetGridDepth()));
	pPathfindingCollisionShape->AddComponent<PhysicsBody>(pPathCollisionShape, 0.f, false, true);
	pPathfindingCollisionShape->SetTag("WaterCollider");

	/* Cache Shell */
	const std::pair<GameObject*, Model*> pShell = Model::CreateObject("Assets/Game/Models/Shell/shell.obj");
	pShell.first->GetTransform()->SetPosition(glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX));

	// Mountain creation data for size 1x1
	// First vector for position, second for rotation
	unsigned Mountain1X1 = 13;
	glm::vec3 Mountain1X1Orientations[] =
	{
		glm::vec3(5, 0, 5),
		glm::vec3(0, 0, 0),
		glm::vec3(15, 0, 5),
		glm::vec3(0, 1.57079633f, 0),
			
		glm::vec3(135, 0, 5),
		glm::vec3(0, 1.57079633f, 0),
		glm::vec3(125, 0, 5),
		glm::vec3(0, 1.57079633f, 0),
		
		glm::vec3(45, 0, 35),
		glm::vec3(0, 1.57079633f, 0),
		glm::vec3(55, 0, 35),
		glm::vec3(0, 1.57079633f, 0),
		glm::vec3(25, 0, 55),
		glm::vec3(0, 0, 0),
		
		glm::vec3(195, 0, 115),
		glm::vec3(0, 0, 0),
		glm::vec3(195, 0, 125),
		glm::vec3(0, 0, 0),
		
		glm::vec3(25, 0, 175),
		glm::vec3(0, 0, 0),
		glm::vec3(45, 0, 165),
		glm::vec3(0, 1.57079633f, 0),
		
		glm::vec3(125, 0, 115),
		glm::vec3(0, 0, 0),
		glm::vec3(125, 0, 105),
		glm::vec3(0, 1.57079633f, 0),
	};
	
	// Mountain creation data for size 1x1
	// First vector for position, second for rotation
	unsigned Mountain1X2 = 12;
	glm::vec3 Mountain1X2Orientations[] =
	{
		glm::vec3(160, 0, 45),
		glm::vec3(0, 1.57079633f, 0),
		glm::vec3(175, 0, 50),
		glm::vec3(0, 0, 0),
		glm::vec3(165, 0, 30),
		glm::vec3(0, 0, 0),
		
		glm::vec3(60, 0, 75),
		glm::vec3(0, 1.57079633f, 0),
		
		glm::vec3(25, 0, 90),
		glm::vec3(0, 0, 0),
		glm::vec3(25, 0, 120),
		glm::vec3(0, 0, 0),
		
		glm::vec3(40, 0, 125),
		glm::vec3(0, 1.57079633f, 0),
		glm::vec3(50, 0, 155),
		glm::vec3(0, 1.57079633f, 0),
		
		glm::vec3(70, 0, 125),
		glm::vec3(0, 1.57079633f, 0),
		glm::vec3(100, 0, 145),
		glm::vec3(0, 1.57079633f, 0),
		
		glm::vec3(150, 0, 165),
		glm::vec3(0, 1.57079633f, 0),
		glm::vec3(175, 0, 150),
		glm::vec3(0, 0, 0),
	};
	
	// Mountain creation data for size 2x2
	// First vector for position, second for rotation
	unsigned Mountain2X2 = 13;
	glm::vec3 Mountain2X2Orientations[] =
	{
		glm::vec3(30, 0, 40),
		glm::vec3(0, 0, 0),
		glm::vec3(140, 0, 50),
		glm::vec3(0, -1.2, 0),
		
		glm::vec3(80, 0, 70),
		glm::vec3(0, 1.57079633f, 0),
		glm::vec3(100, 0, 70),
		glm::vec3(0, 0, 0),
		
		glm::vec3(190, 0, 100),
		glm::vec3(0, 1.57079633f, 0),
		glm::vec3(70, 0, 90),
		glm::vec3(0, 0, 0),
		
		glm::vec3(30, 0, 160),
		glm::vec3(0, 0, 0),
		glm::vec3(10, 0, 120),
		glm::vec3(0, -1.2, 0),
		
		glm::vec3(140, 0, 100),
		glm::vec3(0, 0, 0),
		glm::vec3(140, 0, 120),
		glm::vec3(0, 0, 0),
		
		glm::vec3(90, 0, 130),
		glm::vec3(0, 1.57079633f, 0),
		glm::vec3(110, 0, 160),
		glm::vec3(0, 1.57079633f, 0),
		glm::vec3(170, 0, 170),
		glm::vec3(0, 1.57079633f, 0),
	};

	/* Read mountain */
	const std::string sMountainModelPath = Application::GetInstance()->IsHighQuality() ? "Assets/Game/Models/Mountain/mountain_hq.obj" : "Assets/Game/Models/Mountain/mountain.obj";
	Model* pMountainModel = m_pApplication->GetLoadInterface()->LoadModel(
		sMountainModelPath
	);

	glm::vec3 modelsize = pMountainModel->GetExtents();
	glm::vec3 modelMin, modelMax;

	// Total mountain count for naming mountains
	unsigned MountainCount = 0;

	/* Create mountain 1X1 */
	for (unsigned i = 0; i != Mountain1X1; ++i)
	{
		std::pair<GameObject*, Model*> pMountainPair;
		if (Application::GetInstance()->IsHighQuality()) {
			pMountainPair = Model::CreateObject(
				sMountainModelPath,
				"Assets/Engine/Shaders/Default/PBR.vsh",
				"Assets/Engine/Shaders/Default/PBR_rock.fsh"
			);
		}
		else {
			pMountainPair = Model::CreateObject(
				sMountainModelPath
			);
		}

		GameObject* pMountain = pMountainPair.first;
	
		Transform* pMountainTransform = pMountain->GetTransform();
		pMountainTransform->SetPosition(Mountain1X1Orientations[i * 2]);
		pMountainTransform->SetRotation(Mountain1X1Orientations[i * 2 + 1]);
		pMountainTransform->SetScale(glm::vec3(0.80, 0.85, 0.80));

		pMountain->SetTag("Mountain" + to_string(MountainCount));
		pMountain->AddComponent<PhysicsBody>(pMountainModel, new btBoxShape(btVector3(modelsize.x, modelsize.y, modelsize.z)), 1.f, false, true);

		modelMax = pMountainTransform->GetTransformMatrix() * glm::vec4(modelsize.x, modelsize.y, modelsize.z, 1);
		modelMin = pMountainTransform->GetTransformMatrix() * glm::vec4(-modelsize.x, -modelsize.y, -modelsize.z, 1);

		pathfindsystem->AddStaticNoneTraversable(modelMin, modelMax);
		++MountainCount;
	}
	
	/* Create mountain 1X2 */
	for (unsigned i = 0; i != Mountain1X2; ++i)
	{
		std::pair<GameObject*, Model*> pMountainPair;
		if (Application::GetInstance()->IsHighQuality()) {
			pMountainPair = Model::CreateObject(
				sMountainModelPath,
				"Assets/Engine/Shaders/Default/PBR.vsh",
				"Assets/Engine/Shaders/Default/PBR_rock.fsh"
			);
		}
		else {
			pMountainPair = Model::CreateObject(
				sMountainModelPath
			);
		}

		GameObject* pMountain = pMountainPair.first;
	
		Transform* pMountainTransform = pMountain->GetTransform();
		pMountainTransform->SetPosition(Mountain1X2Orientations[i * 2]);
		pMountainTransform->SetRotation(Mountain1X2Orientations[i * 2 + 1]);
		pMountainTransform->SetScale(glm::vec3(0.80, 0.85, 1.70));

		pMountain->SetTag("Mountain" + to_string(MountainCount));

		pMountain->AddComponent<PhysicsBody>(pMountainModel, new btBoxShape(btVector3(modelsize.x, modelsize.y, modelsize.z)), 1.f, false, true);

		modelMax = pMountainTransform->GetTransformMatrix() * glm::vec4(modelsize.x, modelsize.y, modelsize.z, 1);
		modelMin = pMountainTransform->GetTransformMatrix() * glm::vec4(-modelsize.x, -modelsize.y, -modelsize.z, 1);

		pathfindsystem->AddStaticNoneTraversable(modelMin, modelMax);

		++MountainCount;
	}
	
	/* Create mountain 2X2 */
	for (unsigned i = 0; i != Mountain2X2; ++i)
	{
		std::pair<GameObject*, Model*> pMountainPair;
		if (Application::GetInstance()->IsHighQuality()) {
			pMountainPair = Model::CreateObject(
				sMountainModelPath,
				"Assets/Engine/Shaders/Default/PBR.vsh",
				"Assets/Engine/Shaders/Default/PBR_rock.fsh"
			);
		}
		else {
			pMountainPair = Model::CreateObject(
				sMountainModelPath
			);
		}

		GameObject* pMountain = pMountainPair.first;
	
		Transform* pMountainTransform = pMountain->GetTransform();
		pMountainTransform->SetPosition(Mountain2X2Orientations[i * 2]);
		pMountainTransform->SetRotation(Mountain2X2Orientations[i * 2 + 1]);
		pMountainTransform->SetScale(glm::vec3(1.70, 1.75, 1.70));

		pMountain->SetTag("Mountain" + to_string(MountainCount));

		pMountain->AddComponent<PhysicsBody>(pMountainModel, new btBoxShape(btVector3(modelsize.x, modelsize.y, modelsize.z)), 1.f, false, true);

		modelMax = pMountainTransform->GetTransformMatrix() * glm::vec4(modelsize.x, modelsize.y, modelsize.z, 1);
		modelMin = pMountainTransform->GetTransformMatrix() * glm::vec4(-modelsize.x, -modelsize.y, -modelsize.z, 1);

		pathfindsystem->AddStaticNoneTraversable(modelMin, modelMax);

		++MountainCount;
	}

	/* Delete reference because we used it for reading */
	pMountainModel->DelReference();
}
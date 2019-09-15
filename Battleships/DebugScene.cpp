#include "DebugScene.h"

#include "Application.h"
#include "CameraController.h"

#include "ScriptSystem.h"

#include "Model.h"

#include "HQWaterRenderer.h"
#include "WaterRenderer.h"

#include "Camera.h"
#include "MeshRenderer.h"
#include "NetworkSystem.h"
#include "ObjectSpawners.h"
#include "UnitController.h"
#include "PhysicsBody.h"
#include "BaseShip.h"
#include "AudioSource.h"
#include "RenderSystem.h"
#include "ParticleRenderer.h"
#include "MainMenuBar.h"

/* -------------------- NOTE --------------------  *
 * Please don't submit this class, only use it     *
 * for testing out your own work environment.	   *
 *												   *
 * Use the other scenes to develop for the actual  *
 * release environment.							   *
 *  -------------------- NOTE -------------------- */

void DebugScene::Init()
{
	//Pre load images
	m_preLoadedTextures[0] = Application::GetInstance()->GetLoadInterface()->LoadTexture("Assets/Game/Textures/UI/Loading Screens/splash.jpg");
	m_preLoadedTextures[1] = Application::GetInstance()->GetLoadInterface()->LoadTexture("Assets/Game/Textures/UI/Loading Screens/black.png");

	Application::GetInstance()->GetClient()->SetHost(true);
	m_pNetworkSystem = GetSystem<NetworkSystem>();
	m_pPathfindSystem = GetSystem<PathfindSystem>();

	m_pMainCamera->AddComponent<CameraController>();
	m_pMainCamera->SetTag("Camera");
	
	/* Add Music */
	AudioFile* pBGM = m_pApplication->GetLoadInterface()->LoadAudio("Assets/Game/Sound/BGM/Battle_loop.wav");
	AudioSource* pAudioSource = new AudioSource(m_pMainCamera, pBGM);
	m_pMainCamera->AddComponent(pAudioSource);

	/* Set Water Surface / Border */
	GameObject* pWaterPlane = nullptr;
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

	pWaterPlane->SetTag("Water Surface");

	/* Set Pathfinding Collision Shape*/
	GameObject* pPathfindingCollisionShape = GameObject::Create(glm::vec3(m_pPathfindSystem->GetGridWidth() * 0.5, -1, m_pPathfindSystem->GetGridDepth() * 0.5), glm::vec3(), glm::vec3(1, 1, 1));
	btCollisionShape* pPathCollisionShape = new btBoxShape(btVector3(m_pPathfindSystem->GetGridWidth(), 1, m_pPathfindSystem->GetGridDepth()));
	pPathfindingCollisionShape->AddComponent<PhysicsBody>(pPathCollisionShape, 0.f, false, true);
	pPathfindingCollisionShape->SetTag("WaterCollider");
	
	/* Cache Shell */
	const std::pair<GameObject*, Model*> pShell = Model::CreateObject("Assets/Game/Models/Shell/shell.obj");
	pShell.first->GetTransform()->SetPosition(glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX));

	/* Create Unit Controller */
	GameObject* pUnitController = GameObject::Create();
	pUnitController->SetTag("UnitController");
	UnitController* pUCComponent = new UnitController(pUnitController);
	pUnitController->AddComponent(pUCComponent);
	pUnitController->AddComponent<NetworkIdentity>(1512, Application::GetInstance()->GetClient()->GetGuid());
	
	/* Create Unit Controller */
	GameObject* pUnitControllerEnemy = GameObject::Create();
	pUnitController->SetTag("UnitController");
	UnitController* pUCComponentEnemy = new UnitController(pUnitController);
	pUnitController->AddComponent(pUCComponent);
	pUnitController->AddComponent<NetworkIdentity>(1600, RakNet::RakNetGUID(5312));

	/* Create unit and add to Unit Controller */
	std::pair<GameObject*, Model*> pEnemyShipPair;

	if (Application::GetInstance()->IsHighQuality()) {
		pEnemyShipPair = Model::CreateObject(
			"Assets/Game/Models/Battleships/model.obj",
			"Assets/Engine/Shaders/Default/PBR.vsh",
			"Assets/Engine/Shaders/Default/PBR_ship.fsh"
		);
	}
	else
	{
		pEnemyShipPair = Model::CreateObject(
			"Assets/Game/Models/Battleships/model.obj"
		);
	}

	GameObject* pEnemyShip = pEnemyShipPair.first;
	Model* pEnemyShipModel = pEnemyShipPair.second;

	Transform* pEnemyShipTransform = pEnemyShip->GetTransform();
	pEnemyShipTransform->SetPosition(glm::vec3(75, 0.3, 75));	
	pEnemyShipTransform->SetRotation(glm::radians(glm::vec3(0, 45, 0)));	
	pEnemyShipTransform->SetScale(glm::vec3(5.0, 5.0, 5.0));

	const glm::vec3 modelsize = pEnemyShipModel->GetExtents();

	pEnemyShip->SetTag("Enemy Ship");
	pEnemyShip->AddComponent<PhysicsBody>(pEnemyShipModel, new btBoxShape(btVector3(modelsize.x, modelsize.y, modelsize.z)), 1.f, false, true);

	RakNet::RakNetGUID guid(5312);
	pEnemyShip->AddComponent<NetworkIdentity>(10, guid);
	BaseShip* pBaseShipComponentEnemy = new BaseShip(pEnemyShip);
	pEnemyShip->AddComponent(pBaseShipComponentEnemy);
	pEnemyShip->AddComponent<PathfindAgent>();

	/* Create unit and add to Unit Controller */
	std::pair<GameObject*, Model*> pShipPair;
	
	if (Application::GetInstance()->IsHighQuality()) {
		pShipPair = Model::CreateObject(
			"Assets/Game/Models/Battleships/model.obj",
			"Assets/Engine/Shaders/Default/PBR.vsh",
			"Assets/Engine/Shaders/Default/PBR_ship.fsh"
		);
	}
	else {
		pShipPair = Model::CreateObject(
			"Assets/Game/Models/Battleships/model.obj"
		);
	}

	GameObject* pShip = pShipPair.first;
	Model* pShipModel = pShipPair.second;

	Transform* pShipTransform = pShip->GetTransform();
	pShipTransform->SetPosition(glm::vec3(55, 0.3, 55));	
	pShipTransform->SetRotation(glm::vec3());	
	pShipTransform->SetScale(glm::vec3(5.0, 5.0, 5.0));

	pShip->SetTag("Battleship1");
	pShip->AddComponent<PhysicsBody>(pShipModel, new btBoxShape(btVector3(modelsize.x, modelsize.y, modelsize.z)), 1.f, false, true);
	pShip->AddComponent<NetworkIdentity>(12, Application::GetInstance()->GetClient()->GetGuid());

	BaseShip* pBaseShipComponent = new BaseShip(pShip);
	pShip->AddComponent(pBaseShipComponent);
	pShip->AddComponent<PathfindAgent>();
	
	/* Attach enemy ships to Unit Controller */
	pUCComponentEnemy->AddShip(pBaseShipComponentEnemy);
	
	/* Attach ship to Unit Controller */
	pUCComponent->AddShip(pBaseShipComponent);
	
	const auto enemies = pUCComponentEnemy->GetManagedShips();
	
	pUCComponent->AddEnemyShips(enemies);
	
	/* Create world */
	CreateWorld();

	/* Enable Cursor */
	Application::GetInstance()->GetCursor()->SetActive(true);

	pAudioSource->Play();
}

void DebugScene::DeInit()
{
	/* Disable Cursor */
	Application::GetInstance()->GetCursor()->SetActive(false);
	
	Application::GetInstance()->GetLoadInterface()->DeleteTexture(m_preLoadedTextures[0]->GetPath());
	Application::GetInstance()->GetLoadInterface()->DeleteTexture(m_preLoadedTextures[1]->GetPath());
}

std::vector<ComponentSystem*> DebugScene::GetSystems()
{
	return std::vector<ComponentSystem*> { new NetworkSystem(30.f, GetSpawnFuncs()), new PathfindSystem(200, 10) };
}

void DebugScene::OnUpdate(float a_deltaTime)
{
	glm::vec3 v3CameraPos = m_pMainCamera->GetTransform()->GetPosition();

	v3CameraPos.y = 1;
	v3CameraPos.z += 200;
	RenderSystem::LightObject->GetTransform()->SetPosition(v3CameraPos);

  #ifdef ANDROID
	if (m_pApplication->GetInput()->MousePressed(Input::E_MOUSE_LEFT))
	{
		const glm::ivec2& iv2MousePosition = m_pApplication->GetInput()->GetMousePosition();
		const glm::ivec2& iv2ScreenSize = m_pApplication->GetGLWindow()->GetSize();

		if (iv2MousePosition.x > iv2ScreenSize.x - iv2ScreenSize.x * 0.1f &&
			iv2MousePosition.y < iv2ScreenSize.y * 0.1f)
		{
			MainMenuBar* m = UIRenderer::GetInstance()->GetElement<MainMenuBar>();
			if (m)
				m->SetActive(!m->IsActive());
		}
	}
  #endif
}

void DebugScene::CreateWorld()
{
	/* Create world envoirment */
	
	/* Get pathfind system for creating static objects */
	PathfindSystem* pathfindsystem = GetSystem<PathfindSystem>();
	
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
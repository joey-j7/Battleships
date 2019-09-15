#include "ObjectSpawners.h"
#include <glm/detail/type_vec3.hpp>
#include "NetworkIdentity.h"
#include "MeshRenderer.h"
#include "UnitController.h"
#include "PathfindAgent.h"
#include <bullet/include/LinearMath/btVector3.h>
#include "BaseShip.h"
#include "PhysicsBody.h"
#include "Shell.h"
#include "GameStateManager.h"
#include "ParticleRenderer.h"

std::map<unsigned, std::function<GameObject*(SpawnEvent&, RakNet::BitStream&)>> GetSpawnFuncs()
{
	std::map<unsigned int, std::function<GameObject*(SpawnEvent&, RakNet::BitStream&)>> spawnFuncs;
	spawnFuncs[E_PLAYER] = SpawnPlayer;
	spawnFuncs[E_SHIP] = SpawnShip;
	spawnFuncs[E_SHELL] = SpawnShell;
	spawnFuncs[E_GAME_MANAGER] = SpawnGameManager;
	return spawnFuncs;
}

GameObject* SpawnPlayer(SpawnEvent& a_spawnEvent, RakNet::BitStream& a_extraData)
{
	GameObject* pPlayer = GameObject::Create();
	pPlayer->AddComponent<NetworkIdentity>(a_spawnEvent.IdentityId, a_spawnEvent.Guid);
	pPlayer->SetTag("Player" + std::string(a_spawnEvent.Guid.ToString()));
	pPlayer->AddComponent<UnitController>();

	return pPlayer;
}

GameObject* SpawnShip(SpawnEvent& a_spawnEvent, RakNet::BitStream& a_extraData)
{
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
	
	Transform* pTransform = pShip->GetTransform();
	pTransform->SetPosition(a_spawnEvent.Transform.Position);
	pTransform->SetRotation(a_spawnEvent.Transform.Rotation);
	pTransform->SetScale(a_spawnEvent.Transform.Scale);

	const glm::vec3 modelsize = pShipModel->GetExtents();

	pShip->AddComponent<NetworkIdentity>(a_spawnEvent.IdentityId, a_spawnEvent.Guid);
	pShip->AddComponent<BaseShip>();
	pShip->AddComponent<PathfindAgent>();

	btCollisionShape* collisionshape = new btBoxShape(btVector3(modelsize.x, modelsize.y, modelsize.z));
	pShip->AddComponent<PhysicsBody>(pShipModel, collisionshape, 1.f, false, true);

	pShip->SetTag("Ship" + to_string(a_spawnEvent.IdentityId));

	return pShip;
}

GameObject* SpawnShell(SpawnEvent& a_spawnEvent, RakNet::BitStream& a_extraData)
{
	glm::vec3 shellTarget;
	a_extraData.Read(shellTarget);

	const std::pair<GameObject*, Model*> pModelPair = Model::CreateObject("Assets/Game/Models/Shell/shell.obj");
	GameObject* pShellObj = pModelPair.first;
	Model* pModel = pModelPair.second;

	const glm::vec3 extends = pModel->GetExtents();
	btBoxShape* pBox = new btBoxShape(btVector3(extends.x, extends.y, extends.z));
	
	Transform* pShellTransform = pShellObj->GetTransform();
	pShellTransform->SetPosition(a_spawnEvent.Transform.Position + glm::vec3(0, 1, 0));
	pShellTransform->SetScale(glm::vec3(0.1, 0.1, 0.1));
	
	pShellObj->AddComponent<Shell>(shellTarget);
	pShellObj->AddComponent<NetworkIdentity>(a_spawnEvent.IdentityId, a_spawnEvent.Guid);
	pShellObj->AddComponent<PhysicsBody>(pBox, 1.f, false, true);

	return pShellObj;
}

GameObject* SpawnGameManager(SpawnEvent& a_spawnEvent, RakNet::BitStream& a_extraData)
{
	GameObject* pObj = GameObject::Create();
	pObj->SetTag("StateManager");
	pObj->AddComponent<GameStateManager>();
	NetworkIdentity* pIdentity = new NetworkIdentity(pObj, a_spawnEvent.IdentityId, a_spawnEvent.Guid);
	pIdentity->SetSyncTransform(false);
	pObj->AddComponent(pIdentity);
	return pObj;
}

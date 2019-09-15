#pragma once
#include <functional>
#include <map>
#include "NetworkSystem.h"

enum NetworkObjectType
{
	E_PLAYER,
	E_SHIP,
	E_SHELL,
	E_GAME_MANAGER,
};

std::map<unsigned int, std::function<GameObject*(SpawnEvent&, RakNet::BitStream&)>> GetSpawnFuncs();
GameObject* SpawnPlayer(SpawnEvent& a_spawnEvent, RakNet::BitStream& a_extraData);
GameObject* SpawnShip(SpawnEvent& a_spawnEvent, RakNet::BitStream& a_extraData);
GameObject* SpawnShell(SpawnEvent& a_spawnEvent, RakNet::BitStream& a_extraData);
GameObject* SpawnGameManager(SpawnEvent& a_spawnEvent, RakNet::BitStream& a_extraData);
#pragma once
#include "NetworkBehavior.h"
#include "CameraController.h"
#include "UnitController.h"

struct SpawnEvent;

enum GameStateCommands
{
	E_BEGIN_GAME,
	E_END_GAME
};

enum GameState
{
	E_GAME_LOADING,
	E_GAME_INPROGRESS,
	E_GAME_ENDED,
	E_GAME_LEAVING
};

class GameStateManager : public NetworkBehavior
{
public:
	GameStateManager(GameObject* a_pObj);
	~GameStateManager();

	RakNet::RakString GetName() const override { return "GameStateManager"; }

	void Update() override;
	void Start() override;

private:
	float m_fEndTimer;

	void BeginGame();
	void EndGame(RakNet::BitStream& a_stream);

	void ExitLobby() const;

	static const int MAX_SHIPS = 5;
	unsigned int m_uiShipsSpawned;
	GameState m_currentState;

	CameraController* m_pCameraController;
	UnitController* m_pPlayerUnitController;
	UnitController* m_pEnemyUnitController;

	void OnObjectSpawned(SpawnEvent& a_spawnEvent, GameObject* a_pObj);
};

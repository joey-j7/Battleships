#pragma once
#include "NetworkBehavior.h"

enum LobbyCommand
{
	E_START_GAME,
	E_LEAVE_LOBBY
};

class LobbyWindow;
class MainMenuWindow;
class LobbyManager : public NetworkBehavior
{
public:
	LobbyManager(GameObject* a_pObj, MainMenuWindow* a_pMainMenu, LobbyWindow* a_pLobbyMenu);
	~LobbyManager();

	RakNet::RakString GetName() const override { return "LobbyManager"; }

	/* Network commands */
	void LeaveLobby() const;
	void StartGame() const;

	/* Lobby Events */
	void OnDisconnectPressed() const;
	void OnConnectPressed(std::string a_address) const;
	void OnStartGamePressed() const;

	/* Main menu Events */
	void OnHostLobbyPressed() const;
	void OnJoinGamePressed() const;

private:
	MainMenuWindow* m_pMainMenu;
	LobbyWindow* m_pLobbyMenu;
};

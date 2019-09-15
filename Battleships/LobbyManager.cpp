#include "LobbyManager.h"
#include "MainMenuWindow.h"
#include "LobbyWindow.h"
#include "Debug.h"
#include "MapScene.h"
#include "Application.h"
#include "SceneManager.h"
#include "IntroScene.h"
#include "Camera.h"
#include "AudioSource.h"

LobbyManager::LobbyManager(GameObject* a_pObj, MainMenuWindow* a_pMainMenu, LobbyWindow* a_pLobbyMenu) :
	NetworkBehavior(a_pObj)
{
	m_pMainMenu = a_pMainMenu;
	m_pLobbyMenu = a_pLobbyMenu;

	m_pMainMenu->OnJoinGamePressed.Sub(std::bind(&LobbyManager::OnJoinGamePressed, this), this);
	m_pMainMenu->OnHostLobbyPressed.Sub(std::bind(&LobbyManager::OnHostLobbyPressed, this), this);

	m_pLobbyMenu->OnDisconnectPressed.Sub(std::bind(&LobbyManager::OnDisconnectPressed, this), this);
	m_pLobbyMenu->OnConnectPressed.Sub(std::bind(&LobbyManager::OnConnectPressed, this, std::placeholders::_1), this);
	m_pLobbyMenu->OnStartGamePressed.Sub(std::bind(&LobbyManager::OnStartGamePressed, this), this);

	RegisterCommand(LobbyCommand::E_START_GAME, std::bind(&LobbyManager::StartGame, this));
	RegisterCommand(LobbyCommand::E_LEAVE_LOBBY, std::bind(&LobbyManager::LeaveLobby, this));

	GetClient()->OnConnectionAccepted.Sub([this](NetworkUser&)
	{
		m_pLobbyMenu->SetConnInterfaceEnabled(false);
	}, this);
}

LobbyManager::~LobbyManager()
{
	GetClient()->OnConnectionAccepted.UnSub(this);
}

void LobbyManager::LeaveLobby() const
{
	GetClient()->SetHost(false);
	GetClient()->Disconnect();
	GetClient()->SetAllowIncomingConnection(false);

	m_pLobbyMenu->SetActive(false);
	m_pMainMenu->SetActive(true);

	GetClient()->ClearConnectionMessage();
}

void LobbyManager::StartGame() const
{
	//Create load screen object
	GameObject* pLoadObject = GameObject::Create();
	pLoadObject->AddComponent<LoadScreen>(new IntroScene(Application::GetInstance()), GetCamera()->GetComponent<AudioSource>());
}

void LobbyManager::OnConnectPressed(std::string a_address) const
{
	GetClient()->Connect(a_address);
}

void LobbyManager::OnDisconnectPressed() const
{
	if(GetClient()->IsHost())
		RunCommandAll(LobbyCommand::E_LEAVE_LOBBY);
	else LeaveLobby();
}

void LobbyManager::OnStartGamePressed() const
{
	GetClient()->SetAllowIncomingConnection(true);
	RunCommandAll(LobbyCommand::E_START_GAME);
}

void LobbyManager::OnHostLobbyPressed() const
{
	GetClient()->SetHost(true);
	GetClient()->SetAllowIncomingConnection(true);
	
	m_pMainMenu->SetActive(false);
	m_pLobbyMenu->SetActive(true);
	m_pLobbyMenu->SetConnInterfaceEnabled(false);

	Debug::Log("Lobby started");
}

void LobbyManager::OnJoinGamePressed() const
{
	m_pLobbyMenu->SetActive(true);
	m_pLobbyMenu->SetConnInterfaceEnabled(true);
	m_pMainMenu->SetActive(false);
}
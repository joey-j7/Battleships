#pragma once
#include "UIElement.h"
#include "Event.h"
#include "NetworkClient.h"

class LobbyWindow : public UIElement
{
public:
	LobbyWindow();

	Event<> OnDisconnectPressed;
	Event<std::string> OnConnectPressed;
	Event<> OnStartGamePressed;

	void Render() override;

	void SetConnInterfaceEnabled(bool a_bEnabled) { m_bEnableConnectionInterface = a_bEnabled; }

private:
	bool m_bEnableConnectionInterface;
	NetworkClient* m_pClient;
};

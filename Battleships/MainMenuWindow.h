#pragma once
#include "UIElement.h"
#include "Event.h"

class MainMenuWindow : public UIElement
{
public:
	MainMenuWindow();

	Event<> OnHostLobbyPressed;
	Event<> OnJoinGamePressed;
	Event<> OnDebugPressed;
	Event<> OnSettingsPressed;
	Event<> OnCreditsPressed;

	void Render() override;
};

#pragma once
#include "Scene.h"
#include "Texture.h"

class LobbyWindow;
class MainMenuWindow;
class Transform;

namespace UIElements {
	class GameProperties;
}

class MainMenuScene : public Scene
{
public:
	MainMenuScene(Application* a_pApplication);

	std::vector<ComponentSystem*> GetSystems() override;

	void Init() override;
	void DeInit() override;
	void Pause() override;
	void Resume() override;

	void OnUpdate(float a_deltaTime) override;
	void OnDraw() override {}

private:
	void SetupMenu();
	void SetupWorld();

	void SetLogoPosition() const;

	Texture* m_preLoadedTextures[2];
	LobbyWindow* m_pLobby;
	MainMenuWindow* m_pMainMenu;
	UIElements::GameProperties* m_pSettings;

	Transform* m_pShipTransform[3];
	Transform* m_pLogoTransform;
	GameObject* m_pCamOrbitPivot;
};

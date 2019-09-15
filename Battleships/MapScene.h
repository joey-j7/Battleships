#pragma once
#include "Scene.h"
#include "NetworkClient.h"
#include "LoadScreen.h"

class MapScene : public Scene
{
public:
	MapScene(Application* a_pApplication);

	void Init() override;
	void DeInit() override;
  
	std::vector<ComponentSystem*> GetSystems() override;

	void OnUpdate(float a_deltaTime) override;
	void OnDraw() override {}

private:
	Texture* m_preLoadedTextures[2];
	LoadScreen* m_pLoadScreen;

	NetworkClient* m_pClient;
	bool m_bGameStartInit;
	bool m_bGameInProgress;
	float m_fPollTimer;

	void OnPacketReceived(RakNet::Packet* a_pPacket);

	void InitGame();
	void CreateWorld();
};

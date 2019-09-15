#pragma once

#include "Scene.h"

#include "NetworkSystem.h"
#include "PathfindSystem.h"

/* -------------------- NOTE --------------------  *
 * Please don't submit this class, only use it     *
 * for testing out your own work environment.	   *
 *												   *
 * Use the other scenes to develop for the actual  *
 * release environment.							   *
 *  -------------------- NOTE -------------------- */

class NetworkManager;
class PathfindSystem;

class LoadScreen;
class Texture;

class DebugScene : public Scene
{
public:
	DebugScene(Application* a_pApplication)
		: Scene(a_pApplication) {}
	;

	void Init() override;
	void DeInit() override;
  
	std::vector<ComponentSystem*> GetSystems() override;

	void OnUpdate(float a_deltaTime) override;
	void OnDraw() override {};

private:
	void CreateWorld();

	Texture* m_preLoadedTextures[2];

	NetworkSystem* m_pNetworkSystem;
	PathfindSystem* m_pPathfindSystem;
};

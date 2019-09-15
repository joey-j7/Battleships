#pragma once

#include "Scene.h"

class ParticleScene : public Scene
{
public:
	ParticleScene(Application* a_pApplication) : Scene(a_pApplication) {};

	void Init() override;
  
	std::vector<ComponentSystem*> GetSystems() override;

	void OnUpdate(float a_deltaTime) override {};
	void OnDraw() override {};
};

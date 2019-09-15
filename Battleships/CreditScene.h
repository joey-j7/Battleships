#pragma once
#include "Scene.h"
#include "SpriteRenderer.h"

class CreditScene : public Scene
{
public:
	CreditScene(Application* a_pApp);

	void Init() override;

	void OnUpdate(float a_deltaTime) override;
	void OnDraw() override {}

	void DeInit() override;

private:
	GameObject* m_pSceneLoader;
	SpriteRenderer* m_pCreditsRenderer;
	Texture* m_preLoadedTextures[2];
};

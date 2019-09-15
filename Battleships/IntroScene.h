#pragma once

#include "Scene.h"

class AudioFile;
class SpriteRenderer;
class Texture;

class IntroScene : public Scene
{
public:
	IntroScene(Application* a_pApplication) : Scene(a_pApplication) {};

	void Init() override;
	void DeInit() override;

	void OnUpdate(float a_deltaTime) override;
	void OnDraw() override {};

private:
	Texture* m_preLoadedTextures[2];
	AudioFile* m_pBGM;

	SpriteRenderer* m_pSpriteRenderers[3];
	float m_fSequenceTimer = 0;

	bool m_bEnded = false;
};

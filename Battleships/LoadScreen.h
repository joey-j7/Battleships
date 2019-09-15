#pragma once
#include "BehaviorScript.h"
#include "SpriteRenderer.h"
#include "SoundFader.h"

class LoadScreen : public BehaviorScript
{
public:
	LoadScreen(GameObject* a_pParent, const std::string& a_sImage = "Assets/Game/Textures/UI/Loading Screens/splash.jpg");
	LoadScreen(GameObject* a_pParent, float a_fFadeSpeed, const std::string& a_sImage = "Assets/Game/Textures/UI/Loading Screens/splash.jpg");
	LoadScreen(GameObject* a_pParent, Scene* a_pSceneToLoad, float a_fFadeSpeed = 1.f, const std::string& a_sImage = "Assets/Game/Textures/UI/Loading Screens/splash.jpg");
	LoadScreen(GameObject* a_pParent, Scene* a_pSceneToLoad, AudioSource* a_pAudioSource, float a_fFadeSpeed = 1.f, const std::string& a_sImage = "Assets/Game/Textures/UI/Loading Screens/splash.jpg");

	void Start() override;
	void Update() override;

	void Disable() const;
	void Enable() const;

private:
	void Init(const std::string& a_sImage);

	SoundFader* m_pSoundFader;
	SpriteRenderer* m_pLoadSpriteRenderer;
	SpriteRenderer* m_pBlackSpriteRenderer;

	AudioSource* m_pSourceToFade;
	Scene* m_pSceneToLoad;
	float m_fFadeSpeed;
	float m_fOpacity; 

	bool m_bShouldFade;
};

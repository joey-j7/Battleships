#include "LoadScreen.h"
#include "SpriteRenderer.h"
#include "SceneManager.h"
#include "GameTime.h"

LoadScreen::LoadScreen(GameObject* a_pParent, const std::string& a_sImage) :
	BehaviorScript(a_pParent)
{
	m_pLoadSpriteRenderer = new SpriteRenderer(GetObject(), a_sImage, false);
	GetObject()->AddComponent(m_pLoadSpriteRenderer);

	m_bShouldFade = false;
	m_pSourceToFade = nullptr;
	m_pSceneToLoad = nullptr;
	m_pBlackSpriteRenderer = nullptr;
	m_pSoundFader = nullptr;
	m_fFadeSpeed = 0.f;
	m_fOpacity = 0.f;
}

LoadScreen::LoadScreen(GameObject* a_pParent, float a_fFadeSpeed, const std::string& a_sImage) :
	BehaviorScript(a_pParent)
{
	Init(a_sImage);

	m_bShouldFade = true;
	m_pSourceToFade = nullptr;
	m_pSceneToLoad = nullptr;
	m_fFadeSpeed = a_fFadeSpeed;
	m_fOpacity = 0.f;
}

LoadScreen::LoadScreen(GameObject* a_pParent, Scene* a_pSceneToLoad, float a_fFadeSpeed, const std::string& a_sImage) :
	BehaviorScript(a_pParent)
{
	Init(a_sImage);

	m_bShouldFade = true;
	m_pSourceToFade = nullptr;
	m_pSceneToLoad = a_pSceneToLoad;
	m_fFadeSpeed = a_fFadeSpeed;
	m_fOpacity = 0.f;
}

LoadScreen::LoadScreen(GameObject* a_pParent, Scene* a_pSceneToLoad, AudioSource* a_pAudioSource, float a_fFadeSpeed, const std::string& a_sImage) :
	BehaviorScript(a_pParent)
{
	Init(a_sImage);

	m_bShouldFade = true;
	m_pSourceToFade = a_pAudioSource;
	m_pSceneToLoad = a_pSceneToLoad;
	m_fFadeSpeed = a_fFadeSpeed;
	m_fOpacity = 0.f;
}

void LoadScreen::Init(const std::string& a_sImage)
{
	m_pLoadSpriteRenderer = new SpriteRenderer(GetObject(), a_sImage, false);
	m_pLoadSpriteRenderer->SetEnabled(false);
	m_pLoadSpriteRenderer->SetType(E_RenderLoaders);
	m_pBlackSpriteRenderer = new SpriteRenderer(GetObject(), "Assets/Game/Textures/UI/Loading Screens/black.png", false);
	m_pBlackSpriteRenderer->SetOpacity(0.f);
	m_pBlackSpriteRenderer->SetType(E_RenderLoaders);
	m_pSoundFader = new SoundFader(GetObject());

	GetObject()->AddComponent(m_pLoadSpriteRenderer);
	GetObject()->AddComponent(m_pBlackSpriteRenderer);
	GetObject()->AddComponent(m_pSoundFader);
}

void LoadScreen::Start()
{
	Requires<SpriteRenderer>();

	m_pTransform->SetPosition(glm::vec3(1920 * 0.5f, 1080 * 0.5f, 0.0f));
	m_pTransform->SetScale(glm::vec3(1920, 1080, 1.0f));

	if (m_pSourceToFade)
		m_pSoundFader->FadeOut(m_pSourceToFade, m_fFadeSpeed * 1.2);
}

void LoadScreen::Update()
{
	if (!m_bShouldFade) {
		return;
	}

	m_fOpacity += GameTime::GetDeltaTime() * m_fFadeSpeed;
	m_pBlackSpriteRenderer->SetOpacity(m_fOpacity);
	
	ImGui::GetStyle().Alpha = glm::clamp(1.0f - m_fOpacity, 0.0f, 1.0f);

	if (m_fOpacity >= 1.1f)
	{
		m_pBlackSpriteRenderer->SetEnabled(false);
		m_pLoadSpriteRenderer->SetEnabled(true);
		if (m_pSceneToLoad)
			Application::GetInstance()->GetSceneManager()->LoadScene(m_pSceneToLoad);
	}
}

void LoadScreen::Disable() const
{
	m_pLoadSpriteRenderer->SetEnabled(false);
	if (m_pBlackSpriteRenderer)
		m_pBlackSpriteRenderer->SetEnabled(false);
}

void LoadScreen::Enable() const
{
	m_pLoadSpriteRenderer->SetEnabled(true);
	if (m_pBlackSpriteRenderer)
		m_pBlackSpriteRenderer->SetEnabled(true);
}
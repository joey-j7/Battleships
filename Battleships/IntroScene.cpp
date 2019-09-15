#include "IntroScene.h"

#include "Application.h"

#include "ScriptSystem.h"

#include "Model.h"
#include "Camera.h"
#include "MeshRenderer.h"
#include "AudioSource.h"
#include "RenderSystem.h"
#include "SpriteRenderer.h"
#include "GameTime.h"

#include "SceneManager.h"
#include "DebugScene.h"
#include "Debug.h"
#include "MapScene.h"

void IntroScene::Init()
{
	//Pre load images
	m_preLoadedTextures[0] = Application::GetInstance()->GetLoadInterface()->LoadTexture("Assets/Game/Textures/UI/Loading Screens/splash.jpg");
	m_preLoadedTextures[1] = Application::GetInstance()->GetLoadInterface()->LoadTexture("Assets/Game/Textures/UI/Loading Screens/black.png");

	/* Set Light Animation */
	RenderSystem::LightObject->GetTransform()->SetPosition(glm::vec3(0, 0, -1));
	RenderSystem::LightObject->GetTransform()->AddSequence({
		{
			glm::vec3(0, 0, -1),
			glm::vec3(0, 30, 0),
			glm::vec3(0, 0, 0),
			glm::vec3(0, 0, 0),
			5.3f
		}
	});
	
	/* Add Music */
	m_pBGM = m_pApplication->GetLoadInterface()->LoadAudio("Assets/Game/Sound/BGM/Battle_start.wav");
	AudioSource* pAudioSource = new AudioSource(m_pMainCamera, m_pBGM);
	pAudioSource->SetLooping(false);
	m_pMainCamera->AddComponent(pAudioSource);

	/* Model */
	const std::pair<GameObject*, Model*> pObjectPair = Model::CreateObject(
		"Assets/Game/Models/Evil Man/man.obj",
		"Assets/Engine/Shaders/Default/PBR.vsh",
		"Assets/Engine/Shaders/Default/PBR.fsh"
	);

	GameObject* pObject = pObjectPair.first;

	pObject->GetTransform()->SetPosition(glm::vec3(0, 10, 0));
	pObject->GetTransform()->SetRotation(glm::radians(glm::vec3(0, 180, 0)));
	pObject->SetTag("Evil Man");

	/* Set Sprite Renderers */
	for (int i = 0; i < 3; ++i)
	{
		GameObject* pGameObject = GameObject::Create();
		pGameObject->GetTransform()->SetPosition(glm::vec3(1920.f * 0.5f, 1080.f * 0.5f, 0));

		m_pSpriteRenderers[i] = new SpriteRenderer(pGameObject, "Assets/Game/Textures/Intro/" + to_string(i + 1) + ".jpg", false);	
		m_pSpriteRenderers[i]->SetOpacity(0);
		
		Transform* pTransform = pGameObject->GetTransform();
		pTransform->SetScale(
			glm::vec3(
				1920,
				1080,
				1.0f
			)
		);

		pGameObject->AddComponent(m_pSpriteRenderers[i]);
	}

	/* Set Camera Animation */
	m_pMainCamera->SetTag("Camera");

	m_pMainCamera->GetTransform()->SetPosition(glm::vec3(0, 0, -5));
	m_pMainCamera->GetTransform()->SetRotation(glm::vec3(0, 0, 0));

	m_pMainCamera->GetTransform()->AddSequence({
		{
			glm::vec3(0, 10.0f, -5),
			glm::vec3(0, 0.5f, 2),
			glm::vec3(0, 0, 0),
			glm::vec3(0, 0, 0),
			5.3f
		}
	});

	/* Laugh SFX */
	AudioFile* pLaugh = m_pApplication->GetLoadInterface()->LoadAudio("Assets/Game/Sound/SFX/Evil Man/laugh.wav");
	AudioSource* pAudioLaugh = new AudioSource(pObject, pLaugh);
	pAudioLaugh->SetLooping(false);
	pObject->AddComponent(pAudioLaugh);
	
	/* Play Audio */
	pAudioSource->Play();
	pAudioLaugh->Play();
}

void IntroScene::DeInit()
{
	Application::GetInstance()->GetLoadInterface()->DeleteTexture(m_preLoadedTextures[0]->GetPath());
	Application::GetInstance()->GetLoadInterface()->DeleteTexture(m_preLoadedTextures[1]->GetPath());
}

void IntroScene::OnUpdate(float a_deltaTime)
{
	RenderSystem::LightObject->GetTransform()->PlaySequences();
	m_pMainCamera->GetTransform()->PlaySequences();

	if (m_pMainCamera->GetTransform()->GetSequenceCount() == 0)
	{
		const int iIndex = std::min((int)std::floor(m_fSequenceTimer * 0.7f), 2);
		float fOpacity = m_pSpriteRenderers[iIndex]->GetOpacity();

		if (!m_bEnded && fOpacity < 1.0f)
		{
			fOpacity += GameTime::GetDeltaTime() * 4.0f;
			m_pSpriteRenderers[iIndex]->SetOpacity(fOpacity);

			for (int i = 0; i < iIndex; ++i)
				m_pSpriteRenderers[i]->SetOpacity(0);
		}
		else if (m_fSequenceTimer >= 6.0f)
		{
			m_bEnded = true;

			fOpacity -= GameTime::GetDeltaTime() * 1.0f;
			m_pSpriteRenderers[iIndex]->SetOpacity(fOpacity);

			if (fOpacity <= 0.0f)
			{
				//Create load screen object
				GameObject* pLoadObject = GameObject::Create();
				pLoadObject->AddComponent<LoadScreen>(new MapScene(Application::GetInstance()));
			}
		}

		m_fSequenceTimer += GameTime::GetDeltaTime();
	}
}

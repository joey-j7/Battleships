#include "ParticleScene.h"

#include "Application.h"
#include "ScriptSystem.h"

#include "Camera.h"
#include "ParticleRenderer.h"
#include "CameraController.h"
#include "SkyboxRenderer.h"

void ParticleScene::Init()
{
	Application::GetInstance()->GetClient()->SetHost(true);

	m_pMainCamera->AddComponent<CameraController>();
	m_pMainCamera->SetTag("Camera");

	
	/* Set Skybox */
	const std::string vCubeTextures[6]
	{
		"Assets/Game/Textures/Sky/right.jpg",
		"Assets/Game/Textures/Sky/left.jpg",
		"Assets/Game/Textures/Sky/top.jpg",
		"Assets/Game/Textures/Sky/bottom.jpg",
		"Assets/Game/Textures/Sky/back.jpg",
		"Assets/Game/Textures/Sky/front.jpg"
	};
	
	Texture* pSkyboxCubeMap = m_pApplication->GetLoadInterface()->LoadCubeTexture(vCubeTextures);
	
	GameObject* pSkyboxObject = GameObject::Create();
	pSkyboxObject->AddComponent<SkyboxRenderer>(pSkyboxCubeMap);
	pSkyboxObject->SetTag("Skybox");
}

std::vector<ComponentSystem*> ParticleScene::GetSystems()
{
	return std::vector<ComponentSystem*> { };
}
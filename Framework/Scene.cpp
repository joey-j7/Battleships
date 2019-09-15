#include "Scene.h"
#include "Application.h"
#include "Camera.h"
#include "PhysicsSystem.h"
#include "ScriptSystem.h"
#include "RenderSystem.h"
#include "AudioSystem.h"

Scene::Scene(Application* a_pApplication)
{
	m_pApplication = a_pApplication;
	m_pObjectManager = new ObjectManager();
}

Scene::~Scene()
{
	delete m_pObjectManager;
}

void Scene::AddObject(GameObject* a_pObject) const
{
	m_pObjectManager->AddObject(a_pObject);
}

void Scene::RemoveObject(GameObject* a_pObject) const
{
	m_pObjectManager->MarkForDelete(a_pObject);
}

void Scene::OnCreate()
{
	const glm::uvec2& uv2Size = m_pApplication->GetGLContext()->GetGLWindow()->GetSize();

	PhysicsSystem* pPhysicsSystem = new PhysicsSystem();
	m_pMainCamera = new Camera(45.f, (float)uv2Size.x / (float)uv2Size.y, 0.1f, 9000.f, pPhysicsSystem);

	ScriptSystem* pScriptSystem = new ScriptSystem();
	RenderSystem* pRenderSystem = new RenderSystem();
	AudioSystem* pAudioSystem = new AudioSystem(m_pMainCamera);
	
	std::vector<ComponentSystem*> systems = GetSystems();
	systems.push_back(pScriptSystem);
	systems.push_back(pPhysicsSystem);
	systems.push_back(pAudioSystem);
	systems.push_back(pRenderSystem);
	m_pObjectManager->SetSystems(systems);
  
	m_pObjectManager->AddObject(m_pMainCamera);

	ImGui::GetStyle().Alpha = 1.0f;

	Init();
}

void Scene::Update(float a_fDeltaTime)
{
 	m_pObjectManager->Update();

	OnUpdate(a_fDeltaTime);
}

void Scene::Draw()
{
	OnDraw();
}

void Scene::Unload() const
{
	m_pObjectManager->Unload();
}

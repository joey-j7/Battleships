#pragma once
#include "ObjectManager.h"
#include <vector>

class Camera;
class Application;
class Scene
{
public:
	friend class SceneManager;

	Scene(Application* a_pApplication);
	virtual ~Scene();

	void AddObject(GameObject* a_pObject) const;
	void RemoveObject(GameObject* a_pObject) const;

	virtual std::vector<ComponentSystem*> GetSystems() { return std::vector<ComponentSystem*>(); };

	virtual void Init() {}
	virtual void DeInit() {}

	virtual void Pause() {}
	virtual void Resume() {}

	virtual void OnUpdate(float a_deltaTime) = 0;
	virtual void OnDraw() = 0;

	Camera* GetCamera() const { return m_pMainCamera; }
	const std::vector<GameObject*>& GetPendingGameObjects() const { return m_pObjectManager->GetPendingObjects(); }
	const std::vector<GameObject*>& GetGameObjects() const { return m_pObjectManager->GetObjects(); }

	template <typename T>
	T* GetSystem();

protected:
	Camera* m_pMainCamera = nullptr;
	Application* m_pApplication = nullptr;

private:
	void OnCreate();
	void Update(float a_fDeltaTime);
	void Draw();
	void Unload() const;

	ObjectManager* m_pObjectManager = nullptr;
};

template <typename T>
T* Scene::GetSystem()
{
	for (ComponentSystem* pSystem : m_pObjectManager->GetSystems())
	{
		if (T* pConvertedSystem = dynamic_cast<T*>(pSystem))
			return pConvertedSystem;
	}
	return nullptr;
}
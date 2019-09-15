#pragma once
#include <vector>

class ComponentSystem;
class Component;
class GameObject;
class ObjectManager
{
public:
	ObjectManager();
	~ObjectManager();

	void Unload();

	void SetSystems(std::vector<ComponentSystem*> a_vSystems);
	void AddObject(GameObject* a_pGameObject);
	void MarkForDelete(GameObject* a_pGameObject);
	void DeleteObject(GameObject* a_pGameObject);
	void Update();

	const std::vector<GameObject*>& GetPendingObjects() const { return m_vObjectsToAdd; }
	const std::vector<GameObject*>& GetObjects() const { return m_vGameObjects; }
	const std::vector<ComponentSystem*>& GetSystems() const { return m_vSystems; }

private:
	void Refresh();
	void UpdateComponentsToSystems(const std::vector<Component*>& a_vComponents);

	std::vector<ComponentSystem*> m_vSystems;
	std::vector<GameObject*> m_vGameObjects;
	std::vector<GameObject*> m_vObjectsToAdd;
	std::vector<GameObject*> m_vObjectsToDelete;

	float m_fPhysicsTimer;
};
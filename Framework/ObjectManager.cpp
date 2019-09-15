#include "ObjectManager.h"
#include <vector>
#include <algorithm>
#include "GameObject.h"
#include "Component.h"
#include "ComponentSystem.h"

ObjectManager::ObjectManager()
{
	m_fPhysicsTimer = 0.f;
}

ObjectManager::~ObjectManager()
{
	for (GameObject* pObj : m_vGameObjects)
		delete pObj;

	for (GameObject* pObj : m_vObjectsToAdd)
		delete pObj;

	for (ComponentSystem* pSystem : m_vSystems)
		delete pSystem;
}

void ObjectManager::Unload()
{
	for (GameObject* pObj : m_vGameObjects)
		pObj->Destroy();

	for (GameObject* pObj : m_vObjectsToAdd)
		pObj->Destroy();

	Refresh();
}

void ObjectManager::SetSystems(std::vector<ComponentSystem*> a_vSystems)
{
	m_vSystems = a_vSystems;
}

void ObjectManager::AddObject(GameObject* a_pGameObject)
{
	m_vObjectsToAdd.push_back(a_pGameObject);
}

void ObjectManager::MarkForDelete(GameObject* a_pGameObject)
{
	m_vObjectsToDelete.push_back(a_pGameObject);
}

void ObjectManager::DeleteObject(GameObject* a_pGameObject)
{
	const std::vector<GameObject*>::iterator iter = std::find(m_vGameObjects.begin(), m_vGameObjects.end(), a_pGameObject);
	const std::vector<GameObject*>::iterator iterToAdd = std::find(m_vObjectsToAdd.begin(), m_vObjectsToAdd.end(), a_pGameObject);
	
	if (iter != m_vGameObjects.end())
		m_vGameObjects.erase(iter);

	if (iterToAdd != m_vObjectsToAdd.end())
		m_vObjectsToAdd.erase(iterToAdd);

	delete a_pGameObject;
}

void ObjectManager::Refresh()
{
	if (!m_vObjectsToAdd.empty())
	{
		//Add new objects to game loop
		for (GameObject* pObject : m_vObjectsToAdd)
		{
			if (!pObject->IsDestroyed())
				m_vGameObjects.push_back(pObject);
		}

		std::vector<GameObject*> objectsToAdd(m_vObjectsToAdd);
		m_vObjectsToAdd.clear();

		for (GameObject* pObject : objectsToAdd)
		{
			if (!pObject->IsDestroyed())
			{
				pObject->Refresh();
				UpdateComponentsToSystems(pObject->GetAllComponents());
			}
		}

		for (GameObject* pObject : objectsToAdd)
		{
			if (!pObject->IsDestroyed())
				pObject->Start();
		}
	}

	if (!m_vObjectsToDelete.empty())
	{
		for (GameObject* pObject : m_vObjectsToDelete)
			DeleteObject(pObject);

		m_vObjectsToDelete.clear();
	}
}

void ObjectManager::Update()
{
	for (GameObject* pObject : m_vGameObjects)
	{
		UpdateComponentsToSystems(pObject->m_vComponentsToAdd);
		pObject->Refresh();
	}

	Refresh();

	for (ComponentSystem* pSystem : m_vSystems)
	{
		pSystem->ProcessComponents();
		Refresh();
	}
}

void ObjectManager::UpdateComponentsToSystems(const std::vector<Component*>& a_vComponents)
{
	for (Component* pComponent : a_vComponents)
	{
		for (ComponentSystem* pSystem : m_vSystems)
		{
			if (pSystem->CanProcessComponent(pComponent))
				pSystem->AddComponent(pComponent);
		}
	}
}

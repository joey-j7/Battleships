#include "GameObject.h"
#include "Transform.h"
#include <iostream>
#include "SceneManager.h"
#include "Scene.h"

GameObject::GameObject()
{
	m_pTransform = new Transform(this);
	m_vComponents.push_back(m_pTransform);
	m_bIsUpdating = false;
	m_bEnabled = true;
	m_pParent = nullptr;
	m_bIsDestroyed = false;

	m_pScene = Application::GetInstance()->GetSceneManager()->GetActiveScene();
}

GameObject::~GameObject()
{
	for (Component* pComponent : m_vComponents) {
		delete pComponent;
	}

	for (Component* pComponent : m_vComponentsToAdd) {
		delete pComponent;
	}
}

void GameObject::Refresh()
{
	for (Component* pComponent : m_vComponentsToDelete)
	{
		const std::vector<Component*>::iterator iter = std::find(m_vComponents.begin(), m_vComponents.end(), pComponent);
		if (iter != m_vComponents.end())
		{
			m_vComponents.erase(iter);
			delete pComponent;
		}
	}

	//Add all queued components to the list
	for(Component* pComponent : m_vComponentsToAdd)
		m_vComponents.push_back(pComponent);

	//Query all added components if they need their start method called
	//This is done after all components have been added because in the Start method
	//every component expects all components to be available that were added in the previous frame
	for(Component* pComponent : m_vComponentsToAdd)
	{
		if (m_bIsUpdating)
			pComponent->Start();
	}

	m_vComponentsToAdd.clear();
	m_vComponentsToDelete.clear();

	m_pTransform->Refresh();
}

void GameObject::Start()
{
	m_bIsUpdating = true;
	for (Component* pComponent : m_vComponents)
		pComponent->Start();

	Init(this);
}

void GameObject::OnDestroy()
{
	for (Component* pComponent : m_vComponents)
		pComponent->OnDestroyed();
}

void GameObject::Destroy()
{
	if (m_bIsDestroyed) return;

	m_pScene->RemoveObject(this);
	m_bIsDestroyed = true;
	Destroyed(this);
	OnDestroy();

	for (GameObject* pChild : m_vChildren)
		pChild->Destroy();
}

void GameObject::AddComponent(Component* a_pComponent)
{
	if (std::find(m_vComponents.begin(), m_vComponents.end(), a_pComponent) == m_vComponents.end() &&
		std::find(m_vComponentsToAdd.begin(), m_vComponentsToAdd.end(), a_pComponent) == m_vComponentsToAdd.end())
		m_vComponentsToAdd.push_back(a_pComponent);
}

GameObject* GameObject::GetChild(std::string a_sTag)
{
	for (GameObject* pChild : m_vChildren)
	{
		if (pChild->GetTag() == a_sTag)
			return pChild;
	}
	return nullptr;
}

void GameObject::AddChild(GameObject* a_pObject)
{
	if (a_pObject->GetParent() != this)
		a_pObject->SetParent(this);
	m_vChildren.push_back(a_pObject);
}

void GameObject::RemoveChild(GameObject* a_pObject)
{
	const std::vector<GameObject*>::iterator iter = std::find(m_vChildren.begin(), m_vChildren.end(), a_pObject);
	if (iter != m_vChildren.end())
	{
		a_pObject->SetParent(nullptr);
		m_vChildren.erase(iter);
	}
}

void GameObject::SetParent(GameObject* a_pObject)
{
	if (m_pParent != nullptr)
		m_pParent->RemoveChild(this);
	m_pParent = a_pObject;
	a_pObject->AddChild(this);
}

const GameObject* GameObject::GetRoot() const
{
	if (m_pParent != nullptr)
		return m_pParent->GetRoot();
	return this;
}

GameObject* GameObject::FindGameObjectWithTag(std::string a_sTag)
{
	Scene* pScene = Application::GetInstance()->GetSceneManager()->GetActiveScene();
	for (GameObject* pObject : pScene->GetGameObjects())
	{
		if (pObject->GetTag() == a_sTag)
			return pObject;
	}
	return nullptr;
}

GameObject* GameObject::Create(glm::vec3 a_v3Position, glm::vec3 a_v3Rotation, glm::vec3 a_v3Scale)
{
	return Create(std::move(a_v3Position), glm::quat(a_v3Rotation), std::move(a_v3Scale));
}

GameObject* GameObject::Create(glm::vec3 a_v3Position, glm::quat a_qRotation, glm::vec3 a_v3Scale)
{
	Scene* pScene = Application::GetInstance()->GetSceneManager()->GetActiveScene();
	GameObject* pObject = new GameObject();

	Transform* pTransform = pObject->GetTransform();
	pTransform->SetPosition(a_v3Position);
	pTransform->SetRotation(a_qRotation);
	pTransform->SetScale(a_v3Scale);

	pScene->AddObject(pObject);
	return pObject;
}

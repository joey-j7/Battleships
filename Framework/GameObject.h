#pragma once
#include <vector>
#include <string>
#include <algorithm>
#include <glm/detail/type_vec3.hpp>
#include "Event.h"
#include "Scene.h"
#include "Application.h"

class Component;
class Transform;
class GameObject
{
public:
	friend class ObjectManager;

	GameObject();
	virtual ~GameObject();

	Event<GameObject*> Init;
	Event<GameObject*> Destroyed;

	/// @brief Destroys the GameObject at the start of the next frame, all attached components will also be destroyed
	void Destroy();

	template<typename T, typename ...A>
	void AddComponent(A... a_aArguments);
	void AddComponent(Component* a_pComponent);

	template<typename T>
	void RemoveComponent();

	template<typename T>
	T* GetComponent();

	template<typename T>
	T* GetSystem();

	void SetEnabled(bool a_bEnabled) { m_bEnabled = a_bEnabled; }
	bool IsEnabled() const { return m_bEnabled; }
	bool IsDestroyed() const { return m_bIsDestroyed; }

	const std::vector<GameObject*>& GetChildren() const { return m_vChildren; }
	GameObject* GetChild(unsigned int a_uiIndex) { return m_vChildren[a_uiIndex]; }
	GameObject* GetChild(std::string a_sTag);

	void AddChild(GameObject* a_pObject);
	void RemoveChild(GameObject* a_pObject);
	void SetParent(GameObject* a_pObject);
	GameObject* GetParent() const { return m_pParent; }
	const GameObject* GetRoot() const;

	void SetTag(std::string a_sTag) { m_sTag = a_sTag; }
	std::string GetTag() const { return m_sTag; }

	const std::vector<Component*>& GetAllComponents() const { return m_vComponents; }
	Transform* GetTransform() const { return m_pTransform; }

	static GameObject* FindGameObjectWithTag(std::string a_sTag);

	/// @brief Creates a GameObject and adds it to the active scene, use this function when creating GameObject's instead of new Gameobject
	/// @param a_v3Position	The spawn position
	/// @param a_qRotation The spawn rotation
	/// @param a_v3Scale The spawn scale
	/// @return The created GameObject
	static GameObject* Create(glm::vec3 a_v3Position, glm::quat a_qRotation, glm::vec3 a_v3Scale = glm::vec3(1, 1, 1));
	static GameObject* Create(glm::vec3 a_v3Position = glm::vec3(), glm::vec3 a_v3Rotation = glm::vec3(), glm::vec3 a_v3Scale = glm::vec3(1, 1, 1));

private:
	void Refresh();
	void Start();
	void OnDestroy();

	Transform* m_pTransform = nullptr;
	std::vector<Component*> m_vComponents;
	std::vector<Component*> m_vComponentsToAdd;
	std::vector<Component*> m_vComponentsToDelete;

	std::string m_sTag;
	bool m_bIsUpdating;
	bool m_bIsDestroyed;
	bool m_bEnabled;

	GameObject* m_pParent = nullptr;
	std::vector<GameObject*> m_vChildren;

	Scene* m_pScene;
};

template <typename T, typename ...A>
void GameObject::AddComponent(A... a_arguments)
{
	static_assert(std::is_base_of<Component, T>::value, "T must be of type Component");
	T* pComponent = new T(this, a_arguments...);
	if (std::find(m_vComponents.begin(), m_vComponents.end(), pComponent) == m_vComponents.end() &&
		std::find(m_vComponentsToAdd.begin(), m_vComponentsToAdd.end(), pComponent) == m_vComponentsToAdd.end())
		m_vComponentsToAdd.push_back(pComponent);
}

template <typename T>
void GameObject::RemoveComponent()
{
	std::vector<Component*>::iterator iter = m_vComponents.begin();
	for (; iter != m_vComponents.end(); ++iter)
	{
		if (T* pConvertedComponent = dynamic_cast<T*>(*iter))
		{
			pConvertedComponent->OnDestroyed();
			m_vComponentsToDelete.push_back(pConvertedComponent);
		}
	}
}

template <typename T>
T* GameObject::GetComponent()
{
	for (Component* pComponent : m_vComponents)
	{
		if (T* pConvertedComponent = dynamic_cast<T*>(pComponent))
			return pConvertedComponent;
	}
	return nullptr;
}

template <typename T>
T* GameObject::GetSystem()
{
	return m_pScene->GetSystem<T>();
}

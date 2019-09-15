#pragma once
#include "Event.h"
#include <cassert>
#include "GameObject.h"

class Component
{
public:
	enum Type
	{
		E_Default,
		E_Physics,
		E_Networking,
		E_Audio,
		E_Pathfinding,
		E_RenderSkybox,
		E_RenderBefore,
		E_Render,
		E_RenderWater,
		E_RenderAfter,
		E_RenderParticles,
		E_RenderFrame,
		E_Render2D,
		E_Render2DAfter,
		E_RenderLoaders
	};

	Component(GameObject* a_pParent);
	virtual ~Component() {}

	Event<Component*> Destroyed;

	GameObject* GetObject() const { return m_pObject; }
	bool IsEnabled() const { return m_bEnabled; }
	void SetEnabled(bool a_bEnabled) { m_bEnabled = a_bEnabled; }

	void OnDestroyed();
	virtual void Start() {}

	Type GetType() const {
		return m_eType;
	}

  void SetType(Type a_eType)
  {
    m_eType = a_eType;
  }

	template<typename T>
	T* GetComponent();

	template<typename T>
	T* GetSystem();

	template<typename T>
	void Requires();

protected:
  Type m_eType = E_Default;

private:
	GameObject* m_pObject = nullptr;

	bool m_bEnabled;
};

template <typename T>
T* Component::GetComponent()
{
	return m_pObject->GetComponent<T>();
}

template <typename T>
T* Component::GetSystem()
{
	return m_pObject->GetSystem<T>();
}

template <typename T>
void Component::Requires()
{
	Component* pComponent = m_pObject->GetComponent<T>();
	assert(pComponent != nullptr); // Required component not found
}

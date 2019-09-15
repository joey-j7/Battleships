#pragma once
#include "Component.h"

class ComponentSystem
{
public:
	friend class ObjectManager;
	virtual ~ComponentSystem() = default;

	void AddComponent(Component* a_pComponent);

protected:
	virtual bool CanProcessComponent(Component* a_pComponent) = 0;
	virtual void ProcessComponents() = 0;
	virtual void OnComponentDestroyed(Component* a_pComponent) = 0;
	virtual void OnComponentAdded(Component* a_pComponent) = 0;
};

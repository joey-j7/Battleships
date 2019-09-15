#include "ComponentSystem.h"

void ComponentSystem::AddComponent(Component* a_pComponent)
{
	a_pComponent->Destroyed.Sub(std::bind(&ComponentSystem::OnComponentDestroyed, this, std::placeholders::_1), this);
	OnComponentAdded(a_pComponent);
}
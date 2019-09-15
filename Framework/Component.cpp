#include "Component.h"

Component::Component(GameObject * a_pParent)
{
	m_pObject = a_pParent;
	m_bEnabled = true;
}

void Component::OnDestroyed()
{
	Destroyed(this);
}

#pragma once
#include "ComponentSystem.h"
#include "BehaviorScript.h"

class ScriptSystem : public ComponentSystem
{
protected:
	bool CanProcessComponent(Component* a_pComponent) override;
	void OnComponentAdded(Component* a_pComponent) override;
	void OnComponentDestroyed(Component* a_pComponent) override;

	void ProcessComponents() override;

	void UpdateRoutines(BehaviorScript* a_pScript) const;
	void UpdateInvokeables(BehaviorScript* a_pScript);

private:
	std::vector<BehaviorScript*> m_vComponents;
};

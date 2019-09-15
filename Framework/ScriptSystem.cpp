#include "ScriptSystem.h"
#include "GameTime.h"
#include "PhysicsBody.h"

bool ScriptSystem::CanProcessComponent(Component * a_pComponent)
{
	return dynamic_cast<BehaviorScript*>(a_pComponent);
}

void ScriptSystem::OnComponentAdded(Component* a_pComponent)
{
	BehaviorScript* pScript = static_cast<BehaviorScript*>(a_pComponent);
	m_vComponents.push_back(pScript);

	PhysicsBody* pBody = pScript->GetComponent<PhysicsBody>();
	if (pBody)
		pBody->OnCollision.Sub(std::bind(&BehaviorScript::OnCollision, pScript, std::placeholders::_1), pScript);
}

void ScriptSystem::OnComponentDestroyed(Component* a_pComponent)
{
	BehaviorScript* pScript = static_cast<BehaviorScript*>(a_pComponent);
	const std::vector<BehaviorScript*>::iterator iter = std::find(m_vComponents.begin(), m_vComponents.end(), pScript);
	if (iter != m_vComponents.end())
	{
		PhysicsBody* pBody = pScript->GetComponent<PhysicsBody>();
		if (pBody)
			pBody->OnCollision.UnSub(pScript);

		m_vComponents.erase(iter);
	}
}

void ScriptSystem::ProcessComponents()
{
	for (Component* pComponent : m_vComponents)
	{
		if (!pComponent->IsEnabled()) continue;

		BehaviorScript* pBehaviorScript = static_cast<BehaviorScript*>(pComponent);
		pBehaviorScript->Update();

		UpdateRoutines(pBehaviorScript);
		UpdateInvokeables(pBehaviorScript);
	}
}

void ScriptSystem::UpdateRoutines(BehaviorScript* a_pScript) const
{
	std::vector<Routine*>& vRoutines = a_pScript->m_vRoutines;
	for (std::vector<Routine*>::iterator itRoutine = vRoutines.begin(); itRoutine != vRoutines.end();)
	{
		(*itRoutine)->fTimer += GameTime::GetDeltaTime();
		if ((*itRoutine)->fWaitAmount <= (*itRoutine)->fTimer)
		{
			(*itRoutine)->fTimer = 0;
			(*itRoutine)->fWaitAmount = (*itRoutine)->fFunction();
			if ((*itRoutine)->fWaitAmount == STOP_ROUTINE)
			{
				itRoutine = vRoutines.erase(itRoutine);
				continue;
			}
		}
		++itRoutine;
	}
}

void ScriptSystem::UpdateInvokeables(BehaviorScript* a_pScript)
{
	std::vector<Invokeable*>& vInvokeables = a_pScript->m_vInvokeables;
	for (std::vector<Invokeable*>::iterator iter = vInvokeables.begin(); iter != vInvokeables.end();)
	{
		(*iter)->m_fTimer -= GameTime::GetDeltaTime();
		if ((*iter)->m_fTimer <= 0)
		{
			(*iter)->m_fFunction();
			iter = vInvokeables.erase(iter);
			continue;
		}
		++iter;
	}
}
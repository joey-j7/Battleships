#include "BehaviorScript.h"
#include "Application.h"
#include "SceneManager.h"

BehaviorScript::BehaviorScript(GameObject* a_pParent)
	: Component(a_pParent)
{
	m_pTransform = a_pParent->GetTransform();
	m_pInput = Application::GetInstance()->GetInput();
	Destroyed.Sub([this](Component* a_this)
	{
		OnDestroy();
	}, this);
}

BehaviorScript::~BehaviorScript()
{
	for (Invokeable* pInvokeable : m_vInvokeables)
		delete pInvokeable;

	for (Routine* pRoutine : m_vRoutines)
		delete pRoutine;
}

void BehaviorScript::Destroy() const
{
	GetObject()->Destroy();
}

void BehaviorScript::StartRoutine(std::function<float()> a_function, const char* a_routineName)
{
	m_vRoutines.push_back(new Routine(a_function, a_routineName));
}

void BehaviorScript::StopRoutine(const char* a_name)
{
	for (std::vector<Routine*>::iterator iter = m_vRoutines.begin(); iter != m_vRoutines.end(); ++iter)
	{
		if ((*iter)->sRoutineName == a_name)
		{
			delete *iter;
			m_vRoutines.erase(iter);
			return;
		}
	}
}

void BehaviorScript::Invoke(std::function<void()> a_function, float a_fDelay)
{
	m_vInvokeables.push_back(new Invokeable(a_function, a_fDelay));
}

Camera* BehaviorScript::GetCamera() const
{
	return Application::GetInstance()->GetSceneManager()->GetActiveScene()->GetCamera();
}

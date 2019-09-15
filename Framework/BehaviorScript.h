#pragma once
#include "Component.h"
#include "Transform.h"
#include "InputManager.h"
#include <vector>
#include <functional>

#define STOP_ROUTINE -1

class btManifoldPoint;

struct Invokeable
{
	Invokeable(std::function<void()> a_fFunction, float a_fDelay)
	{
		m_fFunction = a_fFunction;
		m_fTimer = a_fDelay;
	}

	std::function<void()> m_fFunction;
	float m_fTimer;
};

struct Routine
{
	Routine(std::function<float()> a_fFunction, std::string a_sRoutineName)
	{
		fTimer = 0.f;
		fWaitAmount = 0.f;
		fFunction = a_fFunction;
		sRoutineName = a_sRoutineName;
	}

	float fTimer;
	float fWaitAmount;
	std::function<float()> fFunction;
	std::string sRoutineName;
};

class PhysicsBody;
class BehaviorScript : public Component
{
public:
	friend class ScriptSystem;

	BehaviorScript(GameObject* a_pParent);
	~BehaviorScript();

	//Not used because game is capped at 60fps
	//virtual void FixedUpdate() {}

	virtual void Update() {}
	virtual void OnDestroy() {}

	virtual void OnCollision(PhysicsBody* a_pOther) {}

	/// @brief Destroys the current GameObject
	void Destroy() const;

	void StartRoutine(std::function<float()> a_fFunction, const char* a_pcRoutineName);
	void StopRoutine(const char* a_pcName);
	void Invoke(std::function<void()> a_fFunction, float a_fDelay);

	Camera* GetCamera() const;

protected:
	InputManager* m_pInput = nullptr;
	Transform* m_pTransform = nullptr;

private:
	std::vector<Invokeable*> m_vInvokeables;
	std::vector<Routine*> m_vRoutines;
};

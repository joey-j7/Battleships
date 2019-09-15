#pragma once

#include "Application.h"

class Scene;

class Game : public Application
{
public:
	void OnCreate() override;
	void OnUpdate() override;
	void OnDraw() override;

	void ReturnToTitle() override;

private:
	Scene* m_pScene = nullptr;
};

#include "SceneManager.h"
#include "Scene.h"

#include "Application.h"
#include "Texture.h"
#include "VertexStructures.h"

#include "IBO.h"
#include "VBO.h"
#include "Utils.h"

#include "Camera.h"

SceneManager::SceneManager(): m_bRunDefferedFunc(false)
{
}

SceneManager::~SceneManager()
{
	while (!m_vScenes.empty())
	{
		delete m_vScenes.top();
		m_vScenes.pop();
	}
}

void SceneManager::LoadScene(Scene* a_pGameScene)
{
	if (m_bRunDefferedFunc) return;

	if (HasScene())
	{
		Scene* pTopScene = m_vScenes.top();
		pTopScene->DeInit();
	}

	m_defferedFunc = [this, a_pGameScene]()
	{
		if (HasScene())
		{
			Scene* pTopScene = m_vScenes.top();
			pTopScene->Unload();
			delete pTopScene;
			m_vScenes.pop();
		}

		m_vScenes.push(a_pGameScene);
		a_pGameScene->OnCreate();
	};
	m_bRunDefferedFunc = true;
}

void SceneManager::PopScene()
{
	if (m_bRunDefferedFunc) return;

	if (HasScene())
	{
		Scene* pTopScene = m_vScenes.top();
		pTopScene->DeInit();
	}

	m_defferedFunc = [this]()
	{
		if (!HasScene()) return;

		Scene* pTopScene = m_vScenes.top();
		pTopScene->Unload();
		delete pTopScene;
		m_vScenes.pop();

		m_vScenes.top()->Resume();
	};
	m_bRunDefferedFunc = true;
}

void SceneManager::PushScene(Scene* a_pGameScene)
{
	if (m_bRunDefferedFunc) return;

	m_defferedFunc = [this, a_pGameScene]()
	{
		if (HasScene())
			m_vScenes.top()->Pause();

		m_vScenes.push(a_pGameScene);
		a_pGameScene->OnCreate();
	};
	m_bRunDefferedFunc = true;
}

void SceneManager::UpdateScene(float a_deltaTime)
{
	if (HasScene())
		m_vScenes.top()->Update(a_deltaTime);
}

void SceneManager::DrawScene() const
{
	if (HasScene())
		m_vScenes.top()->Draw();
}

bool SceneManager::SwapScenes()
{
	if (m_bRunDefferedFunc)
	{
		m_defferedFunc();
		m_bRunDefferedFunc = false;
		return true;
	}
	return false;
}

bool SceneManager::HasScene() const
{
	return !m_vScenes.empty();
}
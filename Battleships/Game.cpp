#include "Game.h"

#include "SceneManager.h"
#include "LoadScreen.h"

#if DEBUG
#include "MainMenuScene.h"
#else
#include "MainMenuScene.h"
#endif

void Game::OnCreate()
{
#if DEBUG
	m_pScene = new MainMenuScene(this);
#else
	m_pScene = new MainMenuScene(this);
#endif

	m_pSceneManager->LoadScene(m_pScene);
}

void Game::OnUpdate()
{
}

void Game::OnDraw()
{

}

void Game::ReturnToTitle()
{
	GameObject* pLoadObject = GameObject::Create();
	pLoadObject->AddComponent<LoadScreen>(new MainMenuScene(this));
}

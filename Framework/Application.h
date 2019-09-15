#pragma once

#include "NetworkClient.h"
#include "LoadInterface.h"
#include "UIRenderer.h"
#include "InputManager.h"
#include "GLContext.h"
#include "UICursor.h"

class ShaderManager;
class SceneManager;

class Application
{
public:
	Application();
	virtual ~Application();

	virtual void ReturnToTitle() = 0;

#ifdef ANDROID
	void Init(engine* a_pEngine);
	void ClearContext();
#endif

	static Application* GetInstance() { return m_pInstance; }

	GLWindow* GetGLWindow() const { return m_pGLContext->GetGLWindow(); }
	GLContext* GetGLContext() const { return m_pGLContext; }

	SceneManager* GetSceneManager() const { return m_pSceneManager; }
	LoadInterface* GetLoadInterface() const { return m_pLoadInterface; }
	InputManager* GetInput() const { return m_pInput; }
	NetworkClient* GetClient() const { return m_pClient; }

	UIRenderer* GetUIRenderer() const { return m_pUIRenderer; }
	UICursor* GetCursor() const { return m_pUICursor; }

	void SetSplashScreen(std::string a_sPath) { m_sSplashPath = a_sPath; }

	bool IsHighQuality() const { return m_bIsHighQuality; }
	void SetHighQuality(bool a_bIsHighQuality) { m_bIsHighQuality = a_bIsHighQuality; }

	bool ShouldClose() const { return m_bExit; }

	void Run();
	void Exit() { m_bExit = true; }

	virtual void OnCreate() = 0;
	virtual void OnUpdate() = 0;
	virtual void OnDraw() = 0;

protected:
	GLContext* m_pGLContext = nullptr;

	SceneManager* m_pSceneManager = nullptr;
	LoadInterface* m_pLoadInterface = nullptr;
	InputManager* m_pInput = nullptr;
	UIRenderer* m_pUIRenderer = nullptr;
	UICursor* m_pUICursor = nullptr;
	NetworkClient* m_pClient = nullptr;
	
private:
	static Application* m_pInstance;

	bool m_bExit = false;
	std::string m_sSplashPath;

	void DrawSplashScreen() const;

#ifdef WINDOWS
	bool m_bIsHighQuality = true;
#else
	bool m_bIsHighQuality = false;
#endif

#ifdef ANDROID
	bool m_bInitialized = false;
	bool m_bActivated = false;
#endif
};

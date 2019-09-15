#pragma once

#ifdef EGL
#include <EGL/egl.h>
#endif

#include <glm/glm.hpp>
#include "GLWindow.h"

#ifdef ANDROID
#include "engine.h"
#endif

class GLContext
{
public:
#ifdef ANDROID
	GLContext(engine* a_pEngine);
#else
	GLContext();
#endif

	~GLContext();

	static bool HasVSync() { return m_bHasVSync; }
	static void SetVSync(bool a_bEnabled);

	bool IsFullScreen() const { return m_bFullScreen; }
	void SetFullScreen(bool a_bEnabled);

	void Clear();
	static void SetViewport();
	void Swap();

	const glm::uvec2& GetSize() const { return m_uv2Size; }

	static GLWindow* GetGLWindow() { return m_pGLWindow; }

#ifdef EGL
	static EGLDisplay& GetDisplay() { return m_oDisplay; }
	static EGLContext& GetContext() { return m_oContext; }
	static EGLSurface& GetSurface() { return m_oSurface; }

	#ifdef ANDROID
		static void Init();

		static EGLConfig& GetConfig() { return m_oConfig; }
		engine* GetEngine() { return m_pEngine; }
		
		static void KillContext();
		static void KillDisplay();
		static void KillSurface();
	#endif
#elif WINDOWS
	static GLFWmonitor* GetDisplay() { return m_pDisplay; }
#endif

private:
	static GLWindow* m_pGLWindow;

#ifdef EGL
	static EGLDisplay m_oDisplay;
	static EGLContext m_oContext;
	static EGLSurface m_oSurface;

  #ifdef RASPBERRY
	static EGL_DISPMANX_WINDOW_T m_oNativeWindow;
  #elif ANDROID
	static EGLConfig m_oConfig;
	static engine* m_pEngine;
  #endif
#elif WINDOWS
	static GLFWmonitor* m_pDisplay;
#endif

	static glm::uvec2 m_uv2Size;

	bool m_bFullScreen = false;
	static bool m_bHasVSync;
};

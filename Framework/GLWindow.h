#pragma once

#ifdef RASPBERRY
#include <X11/Xlib.h>
#elif ANDROID
#include <EGL/egl.h>
#include <android/native_window.h>
#elif WINDOWS
#include "GLIncludes.h"
#include <glfw3.h>
#endif

#include <glm/glm.hpp>

class GLContext;

class GLWindow
{
public:
	GLWindow(GLContext* a_pGLContext);
	~GLWindow();

	static const glm::ivec2& GetSize() { return m_iv2Size; }
	static const glm::vec2& GetSizeInv() { return m_v2SizeInv; }

	static void SetSize(const glm::ivec2& a_iv2Size);
	static glm::vec2 GetDPIScale() { return m_v2DPIScale; };
	static glm::vec2 GetResolutionScale() { return m_v2ResolutionScale; };

	static float GetScale() { return m_fScale; }

#ifdef RASPBERRY
	static Display* GetDisplay() { return m_pDisplay; }
	static Window* GetWindow() { return &m_Window; }
#elif ANDROID
	void Init();

	static void SetDisplay(EGLDisplay* a_pDisplay) { m_pDisplay = a_pDisplay; }
	static EGLDisplay* GetDisplay() { return &m_pDisplay; }
	static ANativeWindow* GetWindow() { return m_Window; }
#elif WINDOWS
	static GLFWmonitor* GetDisplay() { return m_pDisplay; }
	static GLFWwindow* GetWindow() { return m_pWindow; }

	static void OnResize(GLFWwindow* a_pWindow, int a_iWidth, int a_iHeight);

	void SetFullScreen(bool a_bEnabled);
#endif

private:
	static glm::ivec2 m_iv2Size;
	static glm::vec2 m_v2SizeInv;

	static glm::vec2 m_v2DPIScale;
	static glm::vec2 m_v2ResolutionScale;
	static float m_fScale;

#ifdef RASPBERRY
	static Display* m_pDisplay;
	static Window m_Window;
#elif ANDROID
	static EGLDisplay m_pDisplay;
	static ANativeWindow* m_Window;

	static GLContext* m_pGLContext;
#elif WINDOWS
	static GLFWmonitor* m_pDisplay;
	static GLFWwindow* m_pWindow;

	static GLContext* m_pGLContext;

	/* Used for fullscreen toggle */
	glm::ivec2 m_iv2RestorePosition;
#endif
};
#pragma once

#ifdef WINDOWS

#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw3.h>
#include <glfw3native.h>
#include <ShellScalingApi.h>

#include "GLContext.h"
#include "Debug.h"

GLFWmonitor* GLWindow::m_pDisplay;
GLFWwindow* GLWindow::m_pWindow;
GLContext* GLWindow::m_pGLContext;

GLWindow::GLWindow(GLContext* a_pGLContext)
{
	m_pGLContext = a_pGLContext;
	m_pDisplay = a_pGLContext->GetDisplay();

	// Create the new window.
	if (m_pGLContext->IsFullScreen())
		m_pWindow = glfwCreateWindow(m_pGLContext->GetSize().x, m_pGLContext->GetSize().y, "Battleships", m_pDisplay, NULL);
	else
		m_pWindow = glfwCreateWindow(m_pGLContext->GetSize().x * 0.9, m_pGLContext->GetSize().y * 0.9, "Battleships", NULL, NULL);

	glfwMakeContextCurrent(m_pWindow);

	if (!m_pWindow)
	{
		Debug::Log("Error creating GLFW window!");
		return;
	}

	glfwGetWindowSize(GetWindow(), &m_iv2Size.x, &m_iv2Size.y);
	glfwSetWindowSizeCallback(m_pWindow, OnResize);

	/* Get DPI */
	//const HWND hwnd = glfwGetWin32Window(m_pWindow);
	//
	//if (hwnd) {
		//const auto monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
		//UINT x, y;
		//GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &x, &y);
	//
		///* 1x == 96 DPI */
		//m_v2DPIScale = glm::vec2(x, y);
		//m_v2DPIScale /= 96.0f;
	//}
	
	m_v2SizeInv = glm::vec2(1.0f, 1.0f) / (glm::vec2)m_iv2Size;

	m_v2ResolutionScale = glm::vec2(m_iv2Size.x / 1920.0f, m_iv2Size.y / 1080.0f);
	m_fScale = glm::min(m_v2ResolutionScale.x, m_v2ResolutionScale.y);
	
	Camera::SetOrthoMatrix(glm::ortho<GLfloat>(0.f, m_iv2Size.x, m_iv2Size.y, 0.f));
}

GLWindow::~GLWindow()
{
	glfwDestroyWindow(m_pWindow);
}

void GLWindow::OnResize(GLFWwindow* a_pWindow, int a_iWidth, int a_iHeight)
{
	if (!m_pGLContext->IsFullScreen()) {
		a_iHeight = a_iWidth / 16.f * 9.f;
		glfwSetWindowSize(a_pWindow, a_iWidth, a_iHeight);
	}

	SetSize(glm::ivec2(a_iWidth, a_iHeight));
	m_pGLContext->SetViewport();
}

void GLWindow::SetFullScreen(bool a_bEnabled)
{
	glm::vec2 iv2Size = (glm::vec2)m_pGLContext->GetSize();

	if (a_bEnabled) {
		// Backup window position and size
		glfwGetWindowPos(m_pWindow, &m_iv2RestorePosition.x, &m_iv2RestorePosition.y);
		glfwSetWindowMonitor(m_pWindow, m_pDisplay, 0, 0, iv2Size.x, iv2Size.y, 0);
	}
	else {
		iv2Size *= 0.9f;
		glfwSetWindowMonitor(m_pWindow, nullptr, m_iv2RestorePosition.x, m_iv2RestorePosition.y, iv2Size.x, iv2Size.y, 0);
	}

	SetSize(iv2Size);
	m_pGLContext->SetViewport();
}

#endif
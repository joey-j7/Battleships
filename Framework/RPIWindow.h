#pragma once

#ifdef RASPBERRY

#include "GLContext.h"

#include <X11/Xutil.h>
#include <EGL/egl.h>
#include <X11/Xatom.h>

Display* GLWindow::m_pDisplay;
Window GLWindow::m_Window;

GLWindow::GLWindow(GLContext* a_pGLContext)
{
	// retrieve display
	m_pDisplay = XOpenDisplay(NULL);
	int screen = DefaultScreen(m_pDisplay);
	
	// set window attributes
	Window root = XRootWindow(m_pDisplay, screen);
	XSetWindowAttributes swa;
	
	swa.event_mask =
		ExposureMask |
		KeyPressMask |
		KeyReleaseMask |
		PointerMotionMask |
		ButtonPressMask |
		ButtonReleaseMask;
	swa.override_redirect = true;
	
	// create window
	const glm::uvec2& uv2Size = a_pGLContext->GetSize();
	
	m_Window = XCreateWindow(
		m_pDisplay,
		root,
		0,
		0,
		uv2Size.x,
		uv2Size.y,
		0,
		CopyFromParent,
		InputOnly,
		DefaultVisual(m_pDisplay, screen),
		CWEventMask | CWOverrideRedirect,
		&swa
	);
	
	// make window full screen
	Atom atoms[2] = { XInternAtom(m_pDisplay, "_NET_WM_STATE_FULLSCREEN", False), None };
	XChangeProperty(
		m_pDisplay,
		m_Window,
		XInternAtom(m_pDisplay, "_NET_WM_STATE", False),
		XA_ATOM,
		32,
		PropModeReplace,
		(unsigned char*)atoms,
		1
	);

	// make the window visible on the screen
	XMapWindow(m_pDisplay, m_Window);
	XSync(m_pDisplay, m_Window);
	
	XSetInputFocus(m_pDisplay, m_Window, RevertToParent, CurrentTime);
	XSelectInput(m_pDisplay, m_Window, swa.event_mask);
	XAutoRepeatOff(m_pDisplay);
	
	m_iv2Size = uv2Size;
	
	m_v2SizeInv = glm::vec2(1.0f, 1.0f) / (glm::vec2)m_iv2Size;

	m_v2ResolutionScale = glm::vec2(m_iv2Size.x / 1920.0f, m_iv2Size.y / 1080.0f);
	m_fScale = glm::min(m_v2ResolutionScale.x, m_v2ResolutionScale.y);
}

GLWindow::~GLWindow()
{
	XDestroyWindow(m_pDisplay, m_Window);
	XCloseDisplay(m_pDisplay);
}

#endif
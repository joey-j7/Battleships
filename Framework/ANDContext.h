#pragma once

#ifdef ANDROID

#include "GLContext.h"
#include <EGL/egl.h>

static const EGLint attribute_list[] =
{
	EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
	EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
	EGL_BLUE_SIZE, 8,
	EGL_GREEN_SIZE, 8,
	EGL_RED_SIZE, 8,
	EGL_ALPHA_SIZE, 8,
	EGL_DEPTH_SIZE, 8,
	EGL_STENCIL_SIZE, 8,
	EGL_NONE
};

engine* GLContext::m_pEngine;

EGLDisplay GLContext::m_oDisplay;
EGLContext GLContext::m_oContext;
EGLSurface GLContext::m_oSurface;
EGLConfig GLContext::m_oConfig;

GLContext::GLContext(engine* a_pEngine)
{
	m_pEngine = a_pEngine;
	m_pGLWindow = new GLWindow(this);
	
	Init();
	
#ifdef DEBUG
	SetVSync(false);
#else
	SetVSync(true);
#endif
}

void GLContext::Init()
{
	if (m_oDisplay != EGL_NO_DISPLAY && m_oSurface != EGL_NO_SURFACE && m_oContext != EGL_NO_CONTEXT)
		return;

	if (m_oDisplay == EGL_NO_DISPLAY) {
		Debug::Log("Creating Display...");
		m_oDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
		eglInitialize(m_oDisplay, NULL, NULL);
		
		m_pEngine->display = m_oDisplay;
		
		m_pGLWindow->Init();

		EGL_GET_ERROR();
	}

	if (m_oConfig == nullptr)
	{
		EGLint oNumConfig;
		EGLBoolean bResult = eglChooseConfig(m_oDisplay, attribute_list, &m_oConfig, 1, &oNumConfig);
		EGL_GET_ERROR();
	}

	if (m_oSurface == EGL_NO_SURFACE)
	{
		Debug::Log("Creating Surface...");
		m_oSurface = eglCreateWindowSurface(m_oDisplay, m_oConfig, m_pEngine->app->window, 0);
		
		m_pEngine->surface = m_oSurface;
		EGL_GET_ERROR();
	}

	if (m_oContext == EGL_NO_CONTEXT)
	{
		Debug::Log("Creating Context...");
		EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
		m_oContext = eglCreateContext(m_oDisplay, m_oConfig, 0, contextAttribs);
		
		m_pEngine->context = m_oContext;
		EGL_GET_ERROR();
	}

	GLboolean bResult = eglMakeCurrent(m_oDisplay, m_oSurface, m_oSurface, m_oContext);
	EGL_GET_ERROR();

	eglQuerySurface(m_oDisplay, m_oSurface, EGL_WIDTH, (EGLint*)&(m_uv2Size.x));
	eglQuerySurface(m_oDisplay, m_oSurface, EGL_HEIGHT, (EGLint*)&(m_uv2Size.y));

	m_pGLWindow->SetSize(m_uv2Size);

	m_pEngine->width = m_uv2Size.x;
	m_pEngine->height = m_uv2Size.y;

	m_pEngine->state.angle = 0;
  
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	
	EGL_GET_ERROR();

#ifdef DEBUG
	//	// query egl-specific strings
	//	char *egl_vendor = (char *)eglQueryString(m_oDisplay, EGL_VENDOR);
	//	char *egl_version = (char *)eglQueryString(m_oDisplay, EGL_VERSION);
	//	char *egl_apis = (char *)eglQueryString(m_oDisplay, EGL_CLIENT_APIS);
	//	char *egl_exts = (char *)eglQueryString(m_oDisplay, EGL_EXTENSIONS);
	//
	//	Debug::Log("EGL");
	//	Debug::Log("  Vendor: " + std::string(egl_vendor));
	//	Debug::Log("  Version: " + std::string(egl_version));
	//	Debug::Log("  Client APIs: " + std::string(egl_apis));
	//	Debug::Log("  Extensions: " + std::string(egl_exts));
	//
	//	// query strings
	//	char *vendor = (char *)glGetString(GL_VENDOR);
	//	char *renderer = (char *)glGetString(GL_RENDERER);
	//	char *version = (char *)glGetString(GL_VERSION);
	//	char *glsl_version = (char *)glGetString(GL_SHADING_LANGUAGE_VERSION);
	//	char *exts = (char *)glGetString(GL_EXTENSIONS);
	//
	//	Debug::Log("OpenGL ES");
	//	Debug::Log("  Vendor: " + std::string(vendor));
	//	Debug::Log("  Renderer: " + std::string(renderer));
	//	Debug::Log("  Version: " + std::string(version));
	//	Debug::Log("  GLSL version: " + std::string(glsl_version));
	//	Debug::Log("  Extensions: " + std::string(exts));
#endif
}

GLContext::~GLContext()
{
	delete m_pGLWindow;

	eglMakeCurrent(m_pEngine->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglDestroyContext(m_oDisplay, m_oContext);
	eglDestroySurface(m_oDisplay, m_oSurface);
	eglTerminate(m_oDisplay);

	m_pEngine->animating = 0;
	m_pEngine->display = EGL_NO_DISPLAY;
	m_pEngine->context = EGL_NO_CONTEXT;
	m_pEngine->surface = EGL_NO_SURFACE;

	m_pEngine = nullptr;
	m_oDisplay = nullptr;
	m_oConfig = nullptr;
	m_pGLWindow = nullptr;
	m_oSurface = nullptr;
	m_oContext = nullptr;
}

void GLContext::SetVSync(bool a_bEnabled)
{
	eglSwapInterval(m_oDisplay, a_bEnabled);
	m_bHasVSync = a_bEnabled;
	EGL_GET_ERROR();
}

void GLContext::Swap()
{
	SetViewport();
	eglSwapBuffers(m_oDisplay, m_oSurface);
	EGL_GET_ERROR();
}

void GLContext::KillContext()
{
	Debug::Log("Killing context.");

	// since the context is going away, we have to kill the GL objects
	//KillGLObjects();

	eglMakeCurrent(m_oDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

	if (m_oContext != EGL_NO_CONTEXT) {
		eglDestroyContext(m_oDisplay, m_oContext);
		m_oContext = EGL_NO_CONTEXT;
	}

	Debug::Log("Context killed successfully.");
}

void GLContext::KillSurface()
{
	Debug::Log("Killing surface.");

	eglMakeCurrent(m_oDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

	if (m_oSurface != EGL_NO_SURFACE) {
		eglDestroySurface(m_oDisplay, m_oSurface);
		m_oSurface = EGL_NO_SURFACE;
	}

	Debug::Log("Surface killed successfully.");
}

void GLContext::KillDisplay()
{
	// causes context and surface to go away too, if they are there
    Debug::Log("Killing display.");

	KillContext();
	KillSurface();

	if (m_oDisplay != EGL_NO_DISPLAY) {
		Debug::Log("Terminating display now.");
		eglTerminate(m_oDisplay);
		m_oDisplay = EGL_NO_DISPLAY;
	}

	Debug::Log("Display killed successfully.");
}

#endif
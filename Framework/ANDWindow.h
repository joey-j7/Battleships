#pragma once

#ifdef ANDROID

#include "GLWindow.h"
#include "GLContext.h"

#include <android/configuration.h>
#include <android_native_app_glue.h>
#include <android/native_activity.h>

EGLDisplay GLWindow::m_pDisplay;
ANativeWindow* GLWindow::m_Window;
GLContext* GLWindow::m_pGLContext;

GLWindow::GLWindow(GLContext* a_pGLContext)
{
	m_pGLContext = a_pGLContext;
}

void GLWindow::Init()
{
	m_pDisplay = m_pGLContext->GetDisplay();

	EGLint iFormat;

	bool bResult = eglGetConfigAttrib(m_pDisplay, m_pGLContext->GetConfig(), EGL_NATIVE_VISUAL_ID, &iFormat);
	EGL_GET_ERROR();

	m_Window = m_pGLContext->GetEngine()->app->window;
	ANativeWindow_setBuffersGeometry(m_Window, 0, 0, iFormat);

	/* Get DPI */
	AConfiguration* config = AConfiguration_new();
	AConfiguration_fromAssetManager(config, m_pGLContext->GetEngine()->app->activity->assetManager);
	int32_t density = AConfiguration_getDensity(config);

	/* DENSITY_DEFAULT == 160, DENSITY_HIGH == 240 */
	m_v2DPIScale = glm::vec2(density, density) / 240.0f;
	AConfiguration_delete(config);
}

GLWindow::~GLWindow()
{

}

#endif
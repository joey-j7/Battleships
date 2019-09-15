#pragma once

#ifdef RASPBERRY

#include "GLContext.h"

#include <EGL/egl.h>
#include <bcm_host.h>

static const EGLint attribute_list[] =
{
  EGL_RED_SIZE,
  8,
  EGL_GREEN_SIZE,
  8,
  EGL_BLUE_SIZE,
  8,
  EGL_ALPHA_SIZE,
  8,
  EGL_DEPTH_SIZE,
  5,
  /* 4x MSAA */
  //    EGL_SAMPLES,
  //	4,
  EGL_SURFACE_TYPE,
  EGL_WINDOW_BIT,
  EGL_NONE
};

static const EGLint context_attributes[] =
{
  EGL_CONTEXT_CLIENT_VERSION,
  2,
  EGL_NONE
};

struct gl_limit {
  GLint name;
  char *string;
  int num_args;
};

#define EXPAND(x) x, #x
struct gl_limit limits[] = {
	{ EXPAND(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS), 1 },
	{ EXPAND(GL_MAX_CUBE_MAP_TEXTURE_SIZE), 1 },
	{ EXPAND(GL_MAX_FRAGMENT_UNIFORM_VECTORS), 1 },
	{ EXPAND(GL_MAX_RENDERBUFFER_SIZE), 1 },
	{ EXPAND(GL_MAX_TEXTURE_IMAGE_UNITS), 1 },
	{ EXPAND(GL_MAX_TEXTURE_SIZE), 1 },
	{ EXPAND(GL_MAX_VARYING_VECTORS), 1 },
	{ EXPAND(GL_MAX_VERTEX_ATTRIBS), 1 },
	{ EXPAND(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS), 1 },
	{ EXPAND(GL_MAX_VERTEX_UNIFORM_VECTORS), 1 },
	{ EXPAND(GL_MAX_VIEWPORT_DIMS), 2 },
	{ 0, NULL }
};

EGLDisplay GLContext::m_oDisplay;
EGLContext GLContext::m_oContext;
EGLSurface GLContext::m_oSurface;
EGL_DISPMANX_WINDOW_T GLContext::m_oNativeWindow;

GLContext::GLContext()
{
  bcm_host_init();

  m_oDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);

  EGLBoolean bResult = eglInitialize(m_oDisplay, NULL, NULL);
  assert(EGL_FALSE != bResult);

  EGLConfig oConfig;
  EGLint oNumConfig;
  bResult = eglChooseConfig(m_oDisplay, attribute_list, &oConfig, 1, &oNumConfig);
  assert(EGL_FALSE != bResult);

  bResult = eglBindAPI(EGL_OPENGL_ES_API);
  assert(EGL_FALSE != bResult);

  m_oContext = eglCreateContext(m_oDisplay, oConfig, EGL_NO_CONTEXT, context_attributes);
  assert(m_oContext != EGL_NO_CONTEXT);

  graphics_get_display_size(0, &m_uv2Size[0], &m_uv2Size[1]);

  VC_RECT_T dest_rect;
  dest_rect.x = 0;
  dest_rect.y = 0;
  dest_rect.width = m_uv2Size.x;
  dest_rect.height = m_uv2Size.y;

  VC_RECT_T src_rect;
  src_rect.x = 0;
  src_rect.y = 0;
  src_rect.width = m_uv2Size.x;
  src_rect.height = m_uv2Size.y;

  DISPMANX_DISPLAY_HANDLE_T DispmanDisplayH = vc_dispmanx_display_open(0);
  DISPMANX_UPDATE_HANDLE_T DispmanUpdateH = vc_dispmanx_update_start(0);
  VC_DISPMANX_ALPHA_T DispmanAlphaH =
  {
    DISPMANX_FLAGS_ALPHA_FIXED_ALL_PIXELS,
    255,
    /*alpha 0->255*/
    255
  };

  DISPMANX_ELEMENT_HANDLE_T DispmanElementH = vc_dispmanx_element_add(
    DispmanUpdateH,
    DispmanDisplayH,
    0,
    &dest_rect,
    0,
    &src_rect,
    DISPMANX_PROTECTION_NONE,
    &DispmanAlphaH,
    0,
    (DISPMANX_TRANSFORM_T)0
  );

  Camera::SetOrthoMatrix(glm::ortho<GLfloat>(0.f, m_uv2Size.x, m_uv2Size.y, 0.f));

  m_oNativeWindow.element = DispmanElementH;
  m_oNativeWindow.width = m_uv2Size.x;
  m_oNativeWindow.height = m_uv2Size.y;
  vc_dispmanx_update_submit_sync(DispmanUpdateH);

  m_oSurface = eglCreateWindowSurface(m_oDisplay, oConfig, &(m_oNativeWindow), NULL);
  assert(m_oSurface != EGL_NO_SURFACE);

  bResult = eglMakeCurrent(m_oDisplay, m_oSurface, m_oSurface, m_oContext);
  assert(EGL_FALSE != bResult);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  m_pGLWindow = new GLWindow(this);
#ifdef DEBUG
  /*
  // query egl-specific strings
  char *egl_vendor = (char *)eglQueryString(m_oDisplay, EGL_VENDOR);
  char *egl_version = (char *)eglQueryString(m_oDisplay, EGL_VERSION);
  char *egl_apis = (char *)eglQueryString(m_oDisplay, EGL_CLIENT_APIS);
  char *egl_exts = (char *)eglQueryString(m_oDisplay, EGL_EXTENSIONS);

  Debug::Log("EGL");
  Debug::Log("  Vendor: " + std::string(egl_vendor));
  Debug::Log("  Version: " + std::string(egl_version));
  Debug::Log("  Client APIs: " + std::string(egl_apis));
  Debug::Log("  Extensions: " + std::string(egl_exts));

  // query strings
  char *vendor = (char *)glGetString(GL_VENDOR);
  char *renderer = (char *)glGetString(GL_RENDERER);
  char *version = (char *)glGetString(GL_VERSION);
  char *glsl_version = (char *)glGetString(GL_SHADING_LANGUAGE_VERSION);
  char *exts = (char *)glGetString(GL_EXTENSIONS);

  Debug::Log("OpenGL ES");
  Debug::Log("  Vendor: " + std::string(vendor));
  Debug::Log("  Renderer: " + std::string(renderer));
  Debug::Log("  Version: " + std::string(version));
  Debug::Log("  GLSL version: " + std::string(glsl_version));
  Debug::Log("  Extensions: " + std::string(exts));
  Debug::Log("  Implementation limits:");

  // query limits
  int i = 0;
  for (i = 0; limits[i].name != 0; i++) {
  int param[2] = { 0, 0 };
  glGetIntegerv(limits[i].name, param);

  if (limits[i].num_args == 1)
  Debug::Log("    " + std::string(limits[i].string) + " = " + to_string(param[0]));
  else
  Debug::Log("    " + std::string(limits[i].string) + " = " + to_string(param[0]) + ", " + to_string(param[1]));
  }
  */
  SetVSync(false);
#else
  SetVSync(true);
#endif
}

GLContext::~GLContext()
{
  delete m_pGLWindow;

  eglDestroyContext(m_oDisplay, m_oContext);
  eglDestroySurface(m_oDisplay, m_oSurface);
  eglTerminate(m_oDisplay);
}

void GLContext::SetVSync(bool a_bEnabled)
{
  eglSwapInterval(m_oDisplay, a_bEnabled);
  m_bHasVSync = a_bEnabled;
}

void GLContext::Swap()
{
  SetViewport();
  eglSwapBuffers(m_oDisplay, m_oSurface);
}

#endif
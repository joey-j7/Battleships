#pragma once

#ifdef WINDOWS

#define GLAD_DEBUG

#include <glad/glad.h>
GLFWmonitor* GLContext::m_pDisplay;

GLContext::GLContext()
{
  if (!glfwInit())
  {
    Debug::Log("Error loading GLFW!");
    return;
  }

  /* 4x AA */
  glfwWindowHint(GLFW_SAMPLES, 4);

  m_pDisplay = glfwGetPrimaryMonitor();
  const GLFWvidmode* VidMode = glfwGetVideoMode(m_pDisplay);

  m_uv2Size.x = VidMode->width;
  m_uv2Size.y = VidMode->height;

  m_pGLWindow = new GLWindow(this);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    Debug::Log("Error loading OpenGL context!");
    return;
  }

  //#ifdef GLAD_DEBUG
  //  // before every opengl call call pre_gl_call
  //  glad_set_pre_callback(pre_gl_call);
  //
  //  // post callback checks for glGetError by default
  //
  //  // don't use the callback for glClear
  //  // (glClear could be replaced with your own function)
  //  glad_debug_glClear = glad_glClear;
  //#endif

#ifdef DEBUG
  // query strings
  char *vendor = (char *)glGetString(GL_VENDOR);
  char *renderer = (char *)glGetString(GL_RENDERER);
  char *version = (char *)glGetString(GL_VERSION);
  char *glsl_version = (char *)glGetString(GL_SHADING_LANGUAGE_VERSION);
  char *exts = (char *)glGetString(GL_EXTENSIONS);

  int major = glfwGetWindowAttrib(m_pGLWindow->GetWindow(), GLFW_CONTEXT_VERSION_MAJOR);
  int minor = glfwGetWindowAttrib(m_pGLWindow->GetWindow(), GLFW_CONTEXT_VERSION_MINOR);
  int revision = glfwGetWindowAttrib(m_pGLWindow->GetWindow(), GLFW_CONTEXT_REVISION);

  Debug::Log("OpenGL Version " + to_string(GLVersion.major) + to_string(GLVersion.minor));

  Debug::Log("  Vendor: " + std::string(vendor));
  Debug::Log("  Renderer: " + std::string(renderer));
  Debug::Log("  Version: " + std::string(version));
  Debug::Log("  GLSL version: " + std::string(glsl_version));
  Debug::Log("  Extensions: " + std::string(exts));

  Debug::Log("GLFW Context Version " + to_string(major) + "." + to_string(minor) + "." + to_string(revision));
#endif
}

GLContext::~GLContext()
{
  delete m_pGLWindow;
  glfwTerminate();
}

void GLContext::SetVSync(bool a_bEnabled)
{
  glfwSwapInterval(a_bEnabled);
  m_bHasVSync = a_bEnabled;
}

void GLContext::Swap()
{
  glfwSwapBuffers(m_pGLWindow->GetWindow());
}

#endif
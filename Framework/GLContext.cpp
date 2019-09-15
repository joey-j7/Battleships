#include "Application.h"

#include "Camera.h"

GLWindow* GLContext::m_pGLWindow = nullptr;
glm::uvec2 GLContext::m_uv2Size;

bool GLContext::m_bHasVSync = true;

#include "RPIContext.h"
#include "ANDContext.h"
#include "WINContext.h"

#include <glm/gtc/matrix_transform.hpp>

void GLContext::SetFullScreen(bool a_bEnabled)
{
	m_bFullScreen = a_bEnabled;

#ifdef WINDOWS
	m_pGLWindow->SetFullScreen(a_bEnabled);
#endif
}

void GLContext::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GLContext::SetViewport()
{
	const glm::ivec2& iv2Size = m_pGLWindow->GetSize();
	glViewport(0, 0, iv2Size.x, iv2Size.y);
	
	Camera::SetOrthoMatrix(glm::ortho<GLfloat>(0.f, iv2Size.x, iv2Size.y, 0.f));
}

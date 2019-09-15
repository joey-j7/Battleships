#include "GLWindow.h"
#include "Camera.h"

glm::ivec2 GLWindow::m_iv2Size;
glm::vec2 GLWindow::m_v2SizeInv;

glm::vec2 GLWindow::m_v2DPIScale = glm::vec2(1, 1);
glm::vec2 GLWindow::m_v2ResolutionScale = glm::vec2(1, 1);

float GLWindow::m_fScale = 1.0f;

void GLWindow::SetSize(const glm::ivec2& a_iv2Size)
{
	m_iv2Size = a_iv2Size;
	m_v2SizeInv = glm::vec2(1.0f, 1.0f) / (glm::vec2)a_iv2Size;

	m_v2ResolutionScale = glm::vec2(a_iv2Size.x / 1920.0f, a_iv2Size.y / 1080.0f);
	m_fScale = glm::min(m_v2ResolutionScale.x, m_v2ResolutionScale.y);

	Camera::SetOrthoMatrix(glm::ortho<GLfloat>(0.f, a_iv2Size.x, a_iv2Size.y, 0.f));
}

#include "RPIWindow.h"
#include "ANDWindow.h"
#include "WINWindow.h"
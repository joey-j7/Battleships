#include "IBO.h"
#include <algorithm>

IBO::IBO()
{
	m_uiDataSize = sizeof(GLushort);
	glGenBuffers(1, &m_uiIdentity);
}

IBO::IBO(GLuint a_uiDataCount, GLuint a_uiDataSize, GLvoid* a_pData, GLuint a_uiEntities) {
	Load(a_uiDataCount, a_uiDataSize, a_pData, a_uiEntities);
}

void IBO::Load(GLuint a_uiDataCount, GLuint a_uiDataSize, GLvoid* a_pData, GLuint a_uiEntities)
{
	m_uiDataCount = a_uiDataCount / std::max(a_uiEntities, (GLuint)1);
	m_uiDataSize = a_uiDataSize;

	if (!m_bInitialized) {
		glGenBuffers(1, &m_uiIdentity);
		m_bInitialized = true;
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_uiIdentity);
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER,
		a_uiDataCount * a_uiDataSize,
		a_pData,
		GL_STATIC_DRAW
	);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	GL_GET_ERROR();
}

void IBO::SetBounds(const glm::vec3& a_v3BoundsMin, const glm::vec3& a_v3BoundsMax)
{
	m_v3BoundsMin = a_v3BoundsMin;
	m_v3BoundsMax = a_v3BoundsMax;

	m_v3Extents = (m_v3BoundsMax - m_v3BoundsMin) * 0.5f;
	m_v3Offset = (m_v3BoundsMax + m_v3BoundsMin) * 0.5f;

	m_fRadius = glm::distance(glm::vec3(), m_v3Extents);
}

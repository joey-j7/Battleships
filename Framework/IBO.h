#pragma once

#include "GLIncludes.h"
#include <glm/glm.hpp>
#include "Utils.h"

class IBO
{
public:
	IBO();

	IBO(
		GLuint a_uiDataCount,
		GLuint a_uiDataSize,
		GLvoid* a_pData,
		GLuint a_uiEntities = 1
	);

	virtual ~IBO()
	{
		glDeleteBuffers(1, &m_uiIdentity);
		GL_GET_ERROR();
	}

	void Load(GLuint a_uiDataCount, GLuint a_uiDataSize, GLvoid* a_pData, GLuint a_uiEntities);

	GLuint GetID() const
	{
		return m_uiIdentity;
	}

	GLuint GetCount() const
	{
		return m_uiDataCount;
	}

	void Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_uiIdentity);
	}

	static void Unbind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
  
	void SetBounds(const glm::vec3& a_v3BoundsMin, const glm::vec3& a_v3BoundsMax);

	const glm::vec3& GetExtents() const
	{
		return m_v3Extents;
	}

	const float& GetRadius() const
	{
		return m_fRadius;
	}

	const glm::vec3& GetBoundsMin() const
	{
		return m_v3BoundsMin;
	}

	const glm::vec3& GetBoundsMax() const
	{
		return m_v3BoundsMax;
	}

	const glm::vec3& GetOffset() const
	{
		return m_v3Offset;
	}

private:
	GLuint m_uiIdentity;
	GLuint m_uiDataCount;
	GLuint m_uiDataSize;

	glm::vec3 m_v3BoundsMin;
	glm::vec3 m_v3BoundsMax;

	glm::vec3 m_v3Extents;
	float m_fRadius = 0.0f;

	glm::vec3 m_v3Offset;
	bool m_bInitialized = false;
};

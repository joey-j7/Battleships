#pragma once

#include "GLIncludes.h"
#include <glm/glm.hpp>
#include "VertexStructures.h"

#include "Utils.h"

class VBO
{
public:
	VBO(VertexData::Type a_eType = VertexData::E_DEFAULT)
	{
		m_eType = a_eType;
		glGenBuffers(1, &m_uiIdentity);
	}

	VBO(GLuint a_uiDataCount, VertexData::Type a_eType, GLvoid* a_pData, bool a_bMultiplyCount = true)
	{
		m_eType = a_eType;
		m_uiDataCount = a_uiDataCount;
		m_uiDataSize = VertexData::GetSize(a_eType);

		glGenBuffers(1, &m_uiIdentity);

		glBindBuffer(GL_ARRAY_BUFFER, m_uiIdentity);
		glBufferData(
			GL_ARRAY_BUFFER,
			a_bMultiplyCount ? a_uiDataCount * m_uiDataSize : m_uiDataSize,
			a_pData,
			GL_STATIC_DRAW
		);
 
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		GL_GET_ERROR();
	}

	virtual ~VBO()
	{
		glDeleteBuffers(1, &m_uiIdentity);
	}

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
		glBindBuffer(GL_ARRAY_BUFFER, m_uiIdentity);
	}

	static void Unbind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void SetData(GLsizeiptr size, const GLvoid* data, GLenum usage) const
	{
		glBufferData(GL_ARRAY_BUFFER, size, data, usage);
	}

	VertexData::Type GetType() const
	{
		return m_eType; 
	}
	
private:
	VertexData::Type m_eType = VertexData::E_DEFAULT;

	GLuint m_uiIdentity;
	GLuint m_uiDataCount;
	GLuint m_uiDataSize;
};

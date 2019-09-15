#pragma once

#include "GLIncludes.h"
#include <string>

#include "ReferencedObject.h"

class Texture : public ReferencedObject
{
public:
	Texture() = default;
	virtual ~Texture();

	virtual void Load(
		const std::string& a_sTexturePath,
		unsigned char* a_pucTextureData,
		int a_iWidth,
		int a_iHeight,
		bool a_bRepeat = true,
		bool a_bFilter = true,
		GLint a_iInternalFormat = GL_RGBA,
		GLenum a_eFormat = GL_RGBA,
		GLenum a_eType = GL_UNSIGNED_BYTE
	);

	virtual void Load(
		const std::string& a_sTexturePath,
		bool a_bRepeat = true,
		bool a_bFilter = true,
		GLint a_iInternalFormat = GL_RGBA,
		GLenum a_eFormat = GL_RGBA,
		GLenum a_eType = GL_UNSIGNED_BYTE
	);

	void LoadCube(
		const std::string a_sTexturePath[6],
		bool a_bRepeat = true,
		bool a_bFilter = true,
		GLint a_iInternalFormat = GL_RGB,
		GLenum a_eFormat = GL_RGB,
		GLenum a_eType = GL_UNSIGNED_BYTE
	);
	
	GLuint GetID() const
	{
		return m_uiIdentity;
	}

	GLint GetWidth() const
	{
		return m_iWidth;
	}

	GLint GetHeight() const
	{
		return m_iHeight;
	}

	const std::string& GetPath() const
	{
		return m_sPath;
	}

	void SetPath(const std::string& a_sPath)
	{
		m_sPath = a_sPath;
	}

	void Bind(unsigned int a_uiSlot = 0) const
	{
		glActiveTexture(GL_TEXTURE0 + a_uiSlot);
		glBindTexture(m_eTarget, m_uiIdentity);
	}

	void Unbind() const
	{
		glBindTexture(m_eTarget, 0);
	}

private:
	void SetProperties(GLenum a_eTarget, bool a_bRepeat, bool a_bFilter);

	GLuint m_uiIdentity = 0;

	GLint m_iWidth = 0;
	GLint m_iHeight = 0;
	GLint m_iChannels = 0;
	GLenum m_eTarget = 0;

	std::string m_sPath;
};
#include "Texture.h"
#include "Debug.h"
#include "Utils.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Application.h"

Texture::~Texture()
{
	glDeleteTextures(1, &m_uiIdentity);
}

void Texture::Load(
	const std::string& a_sTexturePath,
	unsigned char* a_pucTextureData,
	int a_iWidth,
	int a_iHeight,
	bool a_bRepeat,
	bool a_bFilter,
	GLint a_iInternalFormat,
	GLenum a_eFormat,
	GLenum a_eType
)
{
	m_sPath = a_sTexturePath;

	m_iWidth = a_iWidth;
	m_iHeight = a_iHeight;

	glGenTextures(1, &m_uiIdentity);
	glBindTexture(GL_TEXTURE_2D, m_uiIdentity);

	SetProperties(GL_TEXTURE_2D, a_bRepeat, a_bFilter);
	glTexImage2D(GL_TEXTURE_2D, 0, a_iInternalFormat, m_iWidth, m_iHeight, 0, a_eFormat, a_eType, a_pucTextureData);

	glBindTexture(GL_TEXTURE_2D, 0);
	GL_GET_ERROR();
  
	m_bIsLoaded = true;
}

void Texture::Load(
	const std::string& a_sTexturePath,
	bool a_bRepeat,
	bool a_bFilter,
	GLint a_iInternalFormat,
	GLenum a_eFormat,
	GLenum a_eType
)
{
	int iLength = 0;
	const char* buffer = LoadFile(a_sTexturePath.c_str(), iLength);

	if (!buffer)
	{
		Debug::Log("Texture failed to load at path: " + a_sTexturePath);
		return;
	};

	unsigned char* pucImageData = stbi_load_from_memory((stbi_uc*)buffer, iLength, &m_iWidth, &m_iHeight, &m_iChannels, STBI_rgb_alpha);
	m_sPath = a_sTexturePath;
	
	glGenTextures(1, &m_uiIdentity);
	glBindTexture(GL_TEXTURE_2D, m_uiIdentity);

	SetProperties(GL_TEXTURE_2D, a_bRepeat, a_bFilter);
	glTexImage2D(GL_TEXTURE_2D, 0, a_iInternalFormat, m_iWidth, m_iHeight, 0, a_eFormat, a_eType, pucImageData);

	glBindTexture(GL_TEXTURE_2D, 0);

	GL_GET_ERROR();
  
	m_bIsLoaded = true;
	stbi_image_free(pucImageData);

  delete[] buffer;
}

/*
	Cubemap texture array structure

	GL_TEXTURE_CUBE_MAP_POSITIVE_X	Right
	GL_TEXTURE_CUBE_MAP_NEGATIVE_X	Left
	GL_TEXTURE_CUBE_MAP_POSITIVE_Y	Top
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Y	Bottom
	GL_TEXTURE_CUBE_MAP_POSITIVE_Z	Back
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Z	Front
 */

void Texture::LoadCube(
	const std::string a_sTexturePath[6],
	bool a_bRepeat,
	bool a_bFilter,
	GLint a_iInternalFormat,
	GLenum a_eFormat,
	GLenum a_eType)
{
	m_sPath = a_sTexturePath[0];

	glGenTextures(1, &m_uiIdentity);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_uiIdentity);
	GL_GET_ERROR();

	int iWidth, iHeight, iChannels;

	for (unsigned int i = 0; i < 6; ++i)
	{
		int iLength = 0;
		const char* buffer = LoadFile(GetPlatformPath(a_sTexturePath[i]).c_str(), iLength);

		if (buffer) 
		{
			unsigned char* pucImageData = stbi_load_from_memory((stbi_uc*)buffer, iLength, &iWidth, &iHeight, &iChannels, STBI_rgb);

			  if (i != 0)
				m_sPath += ";" + a_sTexturePath[i];

			  if (pucImageData)
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, iWidth, iHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, pucImageData);

			  stbi_image_free(pucImageData);
		}
		else
			Debug::Log("Cubemap texture failed to load at path: " + GetPlatformPath(a_sTexturePath[i]));
		
		delete[] buffer;
	}

	m_sPath = GetPlatformPath(m_sPath);

	m_iWidth = iWidth;
	m_iHeight = iHeight;
	m_iChannels = iChannels;
  
	m_bIsLoaded = true;

	SetProperties(GL_TEXTURE_CUBE_MAP, a_bRepeat, a_bFilter);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	Debug::Log("Texture cube loaded at path: " + a_sTexturePath[0]);

	GL_GET_ERROR();
}

void Texture::SetProperties(GLenum a_eTarget, bool a_bRepeat, bool a_bFilter)
{
	m_eTarget = a_eTarget;

	glTexParameteri(a_eTarget, GL_TEXTURE_WRAP_S, a_bRepeat ? GL_REPEAT : GL_CLAMP_TO_EDGE);
	glTexParameteri(a_eTarget, GL_TEXTURE_WRAP_T, a_bRepeat ? GL_REPEAT : GL_CLAMP_TO_EDGE);
	glTexParameteri(a_eTarget, GL_TEXTURE_MIN_FILTER, a_bFilter ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(a_eTarget, GL_TEXTURE_MAG_FILTER, a_bFilter ? GL_LINEAR : GL_NEAREST);
}

#pragma once

#include "GLIncludes.h"

#include <string>
#include "ReferencedObject.h"

class Shader : public ReferencedObject
{
public:
	Shader() = default;
	virtual ~Shader();
	
	void Load(GLenum a_Type, const std::string& a_sShaderPath);

	const GLuint GetID() const
	{
		return m_uiIdentity;
	}

	const std::string& GetPath() const
	{
		return m_sPath;
	}

private:
  void Replace(std::string& source, std::string const& find, std::string const& replace);

	GLuint m_uiIdentity = 0;
	std::string m_sPath = "";
};
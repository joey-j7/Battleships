#pragma once

#include "GLIncludes.h"

#include "ReferencedObject.h"

#include <string>
#include <unordered_map>

class Shader;

class ShaderProgram : public ReferencedObject
{
public:
	struct Data
	{
    GLuint ID;
		GLboolean Normalized;

		GLint Size;
		GLuint Type;

		GLuint DataSize;

		GLuint Offset;
	};

	ShaderProgram() = default;
	virtual ~ShaderProgram();

	void Load(Shader* a_pVertex, Shader* a_pFragment);
	
	const GLuint GetID() const
	{
		return m_uiIdentity;
	}
	
	Shader* GetVertShader() const
	{
		return m_pVertShader;
	}
	
	Shader* GetFragShader() const
	{
		return m_pFragShader;
	}

  void Activate() const
  {
    glUseProgram(m_uiIdentity);
  }

  static void Deactivate()
  {
    glUseProgram(0);
  }

  void DisableAttributes() const
  {
    for (const auto i : m_mAttributes)
    {
      glDisableVertexAttribArray(i.second.ID);
    }
  }

	static void TransformType(Data& a_Attribute);

	GLuint GetAttributeID(const std::string& a_sAttributeName) const;
	GLuint GetUniformID(const std::string& a_sUniformName) const;

  const std::unordered_map<std::string, Data>& GetAttributes() const { return m_mAttributes; };
  const std::unordered_map<std::string, Data>& GetUniforms() const { return m_mUniforms; };
private:
  void GetData(std::unordered_map<std::string, Data>& m_mData, GLenum m_eType);

	std::unordered_map<std::string, Data> m_mAttributes;
	std::unordered_map<std::string, Data> m_mUniforms;

  GLuint m_uiIdentity;

	Shader* m_pVertShader;
	Shader* m_pFragShader;
};
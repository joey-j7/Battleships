#include "ShaderProgram.h"
#include <glm/glm.hpp>

#include "Shader.h"
#include "Debug.h"
#include <malloc.h>
#include "Utils.h"

ShaderProgram::~ShaderProgram()
{
	glDeleteProgram(m_uiIdentity);
}

void ShaderProgram::Load(Shader* a_pVertex, Shader* a_pFragment)
{
	m_uiIdentity = glCreateProgram();
	if (m_uiIdentity == 0) return;
	
	glAttachShader(m_uiIdentity, a_pVertex->GetID());
	glAttachShader(m_uiIdentity, a_pFragment->GetID());
	
	glLinkProgram(m_uiIdentity);
	
	GLint linked;
	glGetProgramiv(m_uiIdentity, GL_LINK_STATUS, &linked);
	
	if (!linked)
	{
		GLint infoLength = 0;
		glGetProgramiv(m_uiIdentity, GL_INFO_LOG_LENGTH, &infoLength);
		
		if (infoLength > 1)
		{
			GLchar* infoLog = (GLchar*)malloc(sizeof(char) * infoLength);
			glGetProgramInfoLog(m_uiIdentity, infoLength, NULL, infoLog);
			
			Debug::Log("Error linking program: " + std::string(infoLog));
			free(infoLog);
		}
		return;
	}
	
	m_bIsLoaded = true;
	GL_GET_ERROR();
	
	m_pVertShader = a_pVertex;
	m_pFragShader = a_pFragment;

	GetData(m_mAttributes, GL_ACTIVE_ATTRIBUTES);
	GetData(m_mUniforms, GL_ACTIVE_UNIFORMS);
}

void ShaderProgram::TransformType(Data& a_Attribute)
{
	/* Ugly type workaround */
	switch (a_Attribute.Type)
	{
	case GL_FLOAT_MAT2:
		a_Attribute.Type = GL_FLOAT;
		a_Attribute.Size *= 4;
		break;
	case GL_FLOAT_VEC2:
		a_Attribute.Type = GL_FLOAT;
		a_Attribute.Size *= 2;
		break;
	case GL_FLOAT_MAT3:
		a_Attribute.Type = GL_FLOAT;
		a_Attribute.Size *= 9;
		break;
	case GL_FLOAT_VEC3:
		a_Attribute.Type = GL_FLOAT;
		a_Attribute.Size *= 3;
		break;
	case GL_FLOAT_MAT4:
		a_Attribute.Type = GL_FLOAT;
		a_Attribute.Size *= 16;
		break;
	case GL_FLOAT_VEC4:
		a_Attribute.Type = GL_FLOAT;
		a_Attribute.Size *= 4;
		break;
	default:
		break;
	}
}

GLuint ShaderProgram::GetAttributeID(const std::string& a_sAttributeName) const
{
	const auto itIter = m_mAttributes.find(a_sAttributeName);

	if (itIter != m_mAttributes.end())
		return itIter->second.ID;

	return 0;
}

GLuint ShaderProgram::GetUniformID(const std::string& a_sUniformName) const
{
	const auto itIter = m_mUniforms.find(a_sUniformName);

	if (itIter != m_mUniforms.end())
		return itIter->second.ID;

	return 0;
}

void ShaderProgram::GetData(std::unordered_map<std::string, Data>& m_mData, GLenum m_eType)
{
	GLint iCount;

	GLint iSize;
	GLenum eType;

	const GLsizei siBuffSize = 32;
	GLchar cName[siBuffSize];
	GLsizei siLength;

	const std::string sTypeName = (m_eType == GL_ACTIVE_ATTRIBUTES ? "Attribute" : (m_eType == GL_ACTIVE_UNIFORMS ? "Uniform" : "Data"));

	glGetProgramiv(m_uiIdentity, m_eType, &iCount);
	/*Debug::Log(
		"\nActive " + sTypeName + " Count"
		": " + to_string(iCount)
	);*/

	for (GLuint i = 0; i < iCount; i++)
	{
		if (m_eType == GL_ACTIVE_ATTRIBUTES)
		{
      glGetActiveAttrib(m_uiIdentity, i, siBuffSize, &siLength, &iSize, &eType, cName);
      m_mData[cName].ID = glGetAttribLocation(m_uiIdentity, cName);
		}
		else if (m_eType == GL_ACTIVE_UNIFORMS)
		{
      glGetActiveUniform(m_uiIdentity, i, siBuffSize, &siLength, &iSize, &eType, cName);
      m_mData[cName].ID = glGetUniformLocation(m_uiIdentity, cName);
		}
		else
		{
			Debug::Log("Unsupported shader component type");
			break;	
		}
		
		m_mData[cName].Normalized = GL_FALSE;
		m_mData[cName].Size = iSize;
		m_mData[cName].Type = eType;

		TransformType(m_mData[cName]);

		const std::string sName = std::string(cName);

		/* Attributes */
		if (sName == "a_position")
			m_mData[cName].Offset = 0;
		else if (sName == "a_texCoord")
			m_mData[cName].Offset = sizeof(glm::vec3);
		else if (sName == "a_normal")
			m_mData[cName].Offset = sizeof(glm::vec3) + sizeof(glm::vec2);

		/* Debug Attributes */
		else if(sName == "a_debugcolor")
			m_mData[cName].Offset = sizeof(glm::vec3);
		
		/* Animation Attributes */
		else if(sName == "a_anim_texCoord")
		{
			m_mData[cName].Offset = 0;	
			m_mData[cName].DataSize = sizeof(glm::vec2);
		}
		else if(sName == "a_anim_normal")
		{
			m_mData[cName].Offset = sizeof(glm::vec2);
			m_mData[cName].DataSize = sizeof(glm::vec3);
		}
		else if(sName == "a_anim_pos1" || sName == "a_anim_pos2")
		{
			m_mData[cName].Offset = sizeof(glm::vec2) + sizeof(glm::vec3);
			m_mData[cName].DataSize = sizeof(glm::vec3);
		}
		
		/*Debug::Log(
			"[" + sTypeName + " " + to_string(i) + "]" + 
			" Type: " + to_string(eType) +
			" Name: " + cName
		);*/
	}
}

#include "Shader.h"

#include "Application.h"
#include <malloc.h>
#include "Debug.h"
#include "Utils.h"

Shader::~Shader()
{
	glDeleteShader(m_uiIdentity);
  GL_GET_ERROR();
}

void Shader::Load(GLenum a_Type, const std::string& a_sShaderPath)
{
	std::string sShaderSource = LoadTextFile(a_sShaderPath.c_str());

	if (sShaderSource.empty())
	{
		Debug::Log("Shader file at path " + a_sShaderPath + " is empty!");
		return;
	}

#ifdef OPENGLES
  sShaderSource.insert(0,
    "#ifdef GL_FRAGMENT_PRECISION_HIGH\nprecision highp float;\n#else\nprecision mediump float;\n#endif\n"
  );
#else
  sShaderSource.insert(0, "#version 120\n");
  //if (a_Type == GL_VERTEX_SHADER) {
  //  Replace(sShaderSource, "attribute", "in");
  //  Replace(sShaderSource, "varying", "out");
  //}
  //else if (a_Type == GL_FRAGMENT_SHADER) {
  //  sShaderSource.insert(0, "out vec4 fragColor;\n");
  //  Replace(sShaderSource, "varying", "in");
  //  Replace(sShaderSource, "gl_FragColor", "fragColor");
  //  Replace(sShaderSource, "texture2D", "texture");
  //  Replace(sShaderSource, "textureCube", "texture");
  //}

  //sShaderSource.insert(0, "#version 120\n");
#endif
	
	m_uiIdentity = glCreateShader(a_Type);
	if (m_uiIdentity == 0) return;

	const char* cShaderSource = sShaderSource.c_str();
	glShaderSource(m_uiIdentity, 1, &cShaderSource, NULL);
	glCompileShader(m_uiIdentity);

	GLint compiled;
	glGetShaderiv(m_uiIdentity, GL_COMPILE_STATUS, &compiled);

	m_sPath = a_sShaderPath;
	m_bIsLoaded = true;

	GL_GET_ERROR();
	
	if (!compiled)
	{
		GLint infoLength = 0;
		glGetShaderiv(m_uiIdentity, GL_INFO_LOG_LENGTH, &infoLength);
				
		if (infoLength > 1)
		{
			char* infoLog = (char*)malloc(sizeof(char) * infoLength);
			glGetShaderInfoLog(m_uiIdentity, infoLength, NULL, infoLog);
			
			Debug::Log("Error compiling the following shader: " + std::string(infoLog));
			free(infoLog);
		}
		
		delete this;
	}
}

void Shader::Replace(std::string& source, std::string const& find, std::string const& replace)
{
  for (std::string::size_type i = 0; (i = source.find(find, i)) != std::string::npos;)
  {
    source.replace(i, find.length(), replace);
    i += replace.length();
  }
}
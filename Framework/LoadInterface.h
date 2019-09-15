#pragma once

#include "ReferenceManager.h"

#include "GLIncludes.h"

#include "Model.h"
#include "Texture.h"

#include "MD2.h"
#include "OBJ.h"
#include "Shape.h"

#include "ShaderProgram.h"
#include "Shader.h"

#include "AudioFile.h"
#include "WAV.h"

class LoadInterface
{
public:
	LoadInterface();
	~LoadInterface();

	/* Models */
	Model* LoadModel(const std::string& a_sFilePath) const;
	void DeleteModel(const std::string& a_sFilePath, Model::Type a_eType) const;
  
	Shape* LoadShape(Shape::Type a_eType, unsigned int a_uiSegments) const;
	void DeleteShape(Shape::Type a_eType, unsigned int a_uiSegments) const;

	/* Textures */
	Texture* LoadTexture(
		const std::string& a_sFilePath,
		bool a_bRepeat = true,
		bool a_bFilter = true,
		GLint a_iInternalFormat = GL_RGBA,
		GLenum a_eFormat = GL_RGBA,
		GLenum a_eType = GL_UNSIGNED_BYTE
	) const;
  
	Texture* LoadCubeTexture(
		const std::string a_sFilePath[6],
		bool a_bRepeat = false,
		bool a_bFilter = true,
		GLint a_iInternalFormat = GL_RGBA,
		GLenum a_eFormat = GL_RGBA,
		GLenum a_eType = GL_UNSIGNED_BYTE
	) const;

	void DeleteTexture(const std::string& a_sFilePath) const
	{
		m_pTextureManager->RemoveReference(a_sFilePath);
	}

	/* Shaders */
	ShaderProgram* LoadShader(const std::string& a_sFilePathVSH, const std::string& a_sFilePathFSH) const;

	void DeleteShader(const std::string& a_sFilePathVSH, const std::string& a_sFilePathFSH) const
	{
		m_pShaderProgramManager->RemoveReference(a_sFilePathVSH + ";" + a_sFilePathFSH);
		m_pShaderManager->RemoveReference(a_sFilePathVSH);
		m_pShaderManager->RemoveReference(a_sFilePathFSH);
	}

	/* Audio */
	AudioFile* LoadAudio(const std::string& a_sFilePath) const;

	void DeleteAudio(const std::string& a_sFilePath) const
	{
		m_pAudioManager->RemoveReference(a_sFilePath);
	}

private:
  std::string m_sAppPath;

	ReferenceManager<MD2>* m_pMD2Manager;
	ReferenceManager<OBJ>* m_pOBJManager;
	ReferenceManager<Shape>* m_pShapeManager;

	ReferenceManager<Texture>* m_pTextureManager;

	ReferenceManager<Shader>* m_pShaderManager;
	ReferenceManager<ShaderProgram>* m_pShaderProgramManager;
  
	ReferenceManager<WAV>* m_pAudioManager;
};

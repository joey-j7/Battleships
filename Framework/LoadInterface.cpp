#include "LoadInterface.h"
#include <algorithm>
#include "Debug.h"
#include "WAV.h"

LoadInterface::LoadInterface()
{
	m_pMD2Manager = new ReferenceManager<MD2>();
	m_pOBJManager = new ReferenceManager<OBJ>();
	m_pShapeManager = new ReferenceManager<Shape>();

	m_pTextureManager = new ReferenceManager<Texture>();
	m_pShaderManager = new ReferenceManager<Shader>();
	m_pShaderProgramManager = new ReferenceManager<ShaderProgram>();

	m_pAudioManager = new ReferenceManager<WAV>();
}

LoadInterface::~LoadInterface()
{
	delete m_pShaderProgramManager;
	delete m_pShaderManager;
	delete m_pTextureManager;

	delete m_pShapeManager;
	delete m_pOBJManager;
	delete m_pMD2Manager;
}

Model* LoadInterface::LoadModel(const std::string& a_sFilePath) const
{
	/* Get Model Data */
	std::string sExtension = a_sFilePath.substr(a_sFilePath.find_last_of(".") + 1);
	std::transform(sExtension.begin(), sExtension.end(), sExtension.begin(), ::tolower);

	std::string refModelName = a_sFilePath;

	if (sExtension  == "obj") {
		OBJ* pOBJ = m_pOBJManager->AddReference(refModelName);
	
		if (!pOBJ->IsLoaded())
			pOBJ->Load(refModelName);

		return pOBJ;
	}
	else if (sExtension  == "md2") {
		MD2* pMD2 = m_pMD2Manager->AddReference(refModelName);
	
		if (!pMD2->IsLoaded())
			pMD2->Load(refModelName);
		
		return pMD2;
	}
	else
	{
		Debug::Log("Model extension " + sExtension + " is not supported");
		return nullptr;
	}
}

void LoadInterface::DeleteModel(const std::string& a_sFilePath, Model::Type a_eType) const
{
	if (a_eType == Model::E_OBJ) m_pOBJManager->RemoveReference(a_sFilePath);
	else if (a_eType == Model::E_MD2) m_pMD2Manager->RemoveReference(a_sFilePath);
	else Debug::Log("Cannot delete: Model extension is not supported");
}

Shape* LoadInterface::LoadShape(Shape::Type a_eType, unsigned a_uiSegments) const
{
	Shape* pShape = nullptr;

	if (a_eType == Shape::E_NONE)
	{
		std::string refQuadName = "NONE;" + to_string(a_uiSegments);
		pShape = m_pShapeManager->AddReference(refQuadName);
	}
	else if (a_eType == Shape::E_QUAD)
	{
		std::string refQuadName = "QUAD;" + to_string(a_uiSegments);
		pShape = m_pShapeManager->AddReference(refQuadName);
	}
	
	if (!pShape->IsLoaded())
		pShape->Load(a_eType, a_uiSegments);

	return pShape;
}

void LoadInterface::DeleteShape(Shape::Type a_eType, unsigned a_uiSegments) const
{
	if (a_eType == Shape::E_QUAD)
		m_pShapeManager->RemoveReference(GetPlatformPath("QUAD;" + to_string(a_uiSegments)));
}

Texture* LoadInterface::LoadTexture(
	const std::string& a_sFilePath,
	bool a_bRepeat,
	bool a_bFilter,
	GLint a_iInternalFormat,
	GLenum a_eFormat,
	GLenum a_eType
) const
{
	std::string refTexName = a_sFilePath;
	Texture* pTexture = m_pTextureManager->AddReference(refTexName);

	if (!pTexture->IsLoaded())
		pTexture->Load(refTexName, a_bRepeat, a_bFilter, a_iInternalFormat, a_eFormat, a_eType);

	return pTexture;
}

Texture* LoadInterface::LoadCubeTexture(
	const std::string a_sFilePath[6],
	bool a_bRepeat,
	bool a_bFilter,
	GLint a_iInternalFormat,
	GLenum a_eFormat,
	GLenum a_eType
) const
{
	std::string sFilePaths;
	
	for (int i = 0; i < 6; ++i)
	{
		sFilePaths += a_sFilePath[i];
		if (i != 5) sFilePaths += ";";
	}

	Texture* pTexture = m_pTextureManager->AddReference(sFilePaths);

	if (!pTexture->IsLoaded())
		pTexture->LoadCube(a_sFilePath, a_bRepeat, a_bFilter, a_iInternalFormat, a_eFormat, a_eType);

	return pTexture;
}

ShaderProgram* LoadInterface::LoadShader(const std::string& a_sFilePathVSH, const std::string& a_sFilePathFSH) const
{
	std::string programRefName = a_sFilePathVSH + ";" + GetPlatformPath(a_sFilePathFSH);
	std::string vshRefName = a_sFilePathVSH;
	std::string fshRefName = a_sFilePathFSH;
	ShaderProgram* pProgram = m_pShaderProgramManager->AddReference(programRefName);
	Shader* pVert = m_pShaderManager->AddReference(vshRefName);
	Shader* pFrag = m_pShaderManager->AddReference(fshRefName);
		
	if (pProgram->IsLoaded())
	{
		return pProgram;
	}
	
	if (!pProgram->IsLoaded())
		pVert->Load(GL_VERTEX_SHADER, vshRefName);
	
	if (!pFrag->IsLoaded())
		pFrag->Load(GL_FRAGMENT_SHADER, fshRefName);
	
	pProgram->Load(pVert, pFrag);

	return pProgram;
}

AudioFile* LoadInterface::LoadAudio(const std::string& a_sFilePath) const
{
	std::string refName = a_sFilePath;
	WAV* pAudio = m_pAudioManager->AddReference(refName);

	if (!pAudio->IsLoaded())
		pAudio->Load(refName);

	return pAudio;
}

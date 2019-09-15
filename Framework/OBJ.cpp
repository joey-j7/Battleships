#include "OBJ.h"
#include "Debug.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "VBO.h"
#include "IBO.h"
#include <climits>
#include <algorithm>
#include "Application.h"

#include "VertexStructures.h"
#include "Utils.h"

void OBJ::Load(const std::string& a_sFilePath)
{
	/* Load new data */
	tinyobj::attrib_t aAttrib;
		
	std::vector<tinyobj::shape_t> vShapes;
	std::vector<tinyobj::material_t> vMaterials;
	
	std::string err;

	m_sFilePath = a_sFilePath;
	m_sFolderPath = GetFolderPath(m_sFilePath);

	const size_t first_slash = a_sFilePath.find_first_of("/");
	std::string sRelFolderPath = GetFolderPath(a_sFilePath);

#ifdef ANDROID
	/* Cache MTL if exist */
	CacheFileToPath((a_sFilePath.substr(0, a_sFilePath.size() - 3) + "mtl").c_str());

	/* Cache OBJ */
	std::string path = CacheFileToPath(a_sFilePath.c_str());
	std::string folderPath = GetFolderPath(path);

	const bool ret = LoadObj(&aAttrib, &vShapes, &vMaterials, &err, path.c_str(), folderPath.c_str());
#else
	sRelFolderPath = sRelFolderPath.substr(first_slash + 1, sRelFolderPath.size());
	const bool ret = LoadObj(&aAttrib, &vShapes, &vMaterials, &err, m_sFilePath.c_str(), m_sFolderPath.c_str());
#endif
	
	if (!ret)
	{
		Debug::Log(err);
		return;
	}

	if (!err.empty())
	{
		Debug::Log(err);
	}
  
	/* We're Going to Need Bounds for Each Shape */
	glm::vec3* pBounds[2];

	pBounds[0] = new glm::vec3[vShapes.size()];
	pBounds[1] = new glm::vec3[vShapes.size()];

	/* Used to predict number of vertices (maximum number of posibilities) */
	GLuint uiIndexCount = 0;

	for (int i = 0; i < vShapes.size(); ++i)
	{
		uiIndexCount += vShapes[i].mesh.indices.size();
	}

	/* Retrieve Data*/
	std::unordered_map<unsigned long long int, GLuint> mHashMap;

	VertexData::Default* pVertices = new VertexData::Default[uiIndexCount];
	m_uiVertexCount = 0;

	/* Required for Bullet */
	glm::vec3* pPositionData = new glm::vec3[uiIndexCount];
  
	const GLint vSize = aAttrib.vertices.size();
	const GLint nSize = aAttrib.normals.size();
	const GLint tSize = aAttrib.texcoords.size();

	for (GLuint i = 0; i < vShapes.size(); ++i)
	{
		auto& a_Indices = vShapes[i].mesh.indices;
		GLuint uiShapeVertices = 0;

		for (GLuint j = 0; j < a_Indices.size(); ++j)
		{
			auto& ind = a_Indices[j];

			const GLint vID = 3 * ind.vertex_index;
			const GLint nID = 3 * ind.normal_index;
			const GLint tID = 2 * ind.texcoord_index;

			const bool bHasPosition = vID >= 0 && vID + 2 < vSize;

			if (!bHasPosition)
				continue;

			/* Create hash based on three IDs */
			const GLushort vIDs = ind.vertex_index;
			const GLushort nIDs = ind.normal_index;
			const GLushort tIDs = ind.texcoord_index;

			unsigned long long int lHash = vIDs;
			lHash <<= sizeof(GLushort) * 8;
			lHash += nIDs;
			lHash <<= sizeof(GLushort) * 8;
			lHash += tIDs;

			/* Check if created hash already exists, otherwise remap index */
			const auto it = mHashMap.find(lHash);
			if (it == mHashMap.end())
			{
				/* Generate Data */
				VertexData::Default vertexData;

				vertexData.Position.x = aAttrib.vertices[vID + 0];
				vertexData.Position.y = aAttrib.vertices[vID + 1];
				vertexData.Position.z = aAttrib.vertices[vID + 2];

				/* Get Smallest and Biggest Values */
				pBounds[0][i] = glm::min(vertexData.Position, pBounds[0][i]);
				pBounds[1][i] = glm::max(vertexData.Position, pBounds[1][i]);
 
				if (nID >= 0 && nID + 2 < nSize)
				{
					vertexData.Normal.x = aAttrib.normals[nID + 0];
					vertexData.Normal.y = aAttrib.normals[nID + 1];
					vertexData.Normal.z = aAttrib.normals[nID + 2];
				}
 
				if (tID >= 0 && tID + 1 < tSize)
				{
					vertexData.UV.x = aAttrib.texcoords[tID + 0];
					vertexData.UV.y = 1.0f - aAttrib.texcoords[tID + 1];
				}

				/* Set Data */
				const GLuint iSize = mHashMap.size();
				mHashMap[lHash] = iSize;

				ind.vertex_index = m_uiVertexCount;

				pVertices[m_uiVertexCount] = vertexData;
				pPositionData[m_uiVertexCount] = vertexData.Position;
				
				m_uiVertexCount++;
				uiShapeVertices++;
			}
			else
				ind.vertex_index = it->second;
		}
	}
  
	if (m_uiVertexCount >= USHRT_MAX)
		Debug::Log("Warning! Model exceeds maximum index count: " + m_sFilePath);

	/* Generate VBO */
	m_pVBO = new VBO(m_uiVertexCount, VertexData::Type::E_DEFAULT, pVertices);

	/* Generate IBOs */
	m_vMeshes.reserve(vShapes.size());

	for (int i = 0; i < vShapes.size(); ++i)
	{
		auto& vShape = vShapes[i];

		/* Prepare IBO */
		uiIndexCount = vShape.mesh.indices.size();
		GLushort* pIndices = new GLushort[uiIndexCount];

		for (int j = 0; j < uiIndexCount; ++j)
		{
			pIndices[j] = vShape.mesh.indices[j].vertex_index;
		}

		/* Retrieve textures */
		std::vector<int> aMaterials = vShape.mesh.material_ids;
		aMaterials.erase(std::unique(aMaterials.begin(), aMaterials.end()), aMaterials.end());

		std::vector<Texture*> vTextures;

		/* Only support one texture per mesh */
		for (int j = 0; j < std::min((int)aMaterials.size(), 1); ++j)
		{
			if (aMaterials[j] >= 0)
			{
				const GLuint uiMaterialID = aMaterials[j];
				const std::string name = vMaterials[uiMaterialID].diffuse_texname;

				if (!name.empty())
				{
					Texture* pTexture = Application::GetInstance()->GetLoadInterface()->LoadTexture(sRelFolderPath + vMaterials[uiMaterialID].diffuse_texname);

					if (!pTexture->IsLoaded())
						continue;

					vTextures.push_back(
						pTexture
					);
					
					/* Check for LQ texture */
					const size_t siSize = vMaterials[uiMaterialID].diffuse_texname.size();
					
					std::string sLQ = vMaterials[uiMaterialID].diffuse_texname;
					sLQ = sLQ.substr(0, siSize - 4);
					
					std::string sExt = vMaterials[uiMaterialID].diffuse_texname;
					sExt = sExt.substr(siSize - 4, siSize);

					Texture* pLQ = Application::GetInstance()->GetLoadInterface()->LoadTexture(sRelFolderPath + sLQ + "_lq" + sExt);

					if (pLQ->IsLoaded())
						vTextures.push_back(pLQ);
				}
			}
		}

		/* Default to white texture */
		if (vTextures.empty())
		{
			vTextures.push_back(
				Application::GetInstance()->GetLoadInterface()->LoadTexture(
					"Assets/Engine/Textures/white.png"
				)
			);
		}
  
		/* Generate OpenGL buffers */
		IBO* pIBO = new IBO(uiIndexCount, sizeof(GLushort), pIndices);
		
		/* Set bounds and extents */
		pIBO->SetBounds(pBounds[0][i], pBounds[1][i]);

		/* Add mesh to model */
		const std::unordered_map <std::string, Mesh::Animation> map;
		m_vMeshes.push_back(new Mesh(m_vMeshes.size(), pIBO, map, vTextures));
    GL_GET_ERROR();
	}
  
	delete[] pBounds[0];
	delete[] pBounds[1];

	delete[] pVertices;
  
	m_Type = Type::E_OBJ;
	m_bIsLoaded = true;
	
	CalculateBounds();
}

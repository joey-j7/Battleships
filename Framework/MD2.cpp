#include "MD2.h"

#include <fstream>

#include "VBO.h"
#include "IBO.h"

#include "Application.h"
#include <cfloat>
#include "Debug.h"
#include "Utils.h"

void MD2::Load(const std::string& a_sFilePath)
{
	Debug::Log("\n-- Loading MD2 with file path: " + a_sFilePath + " --");

	m_sFilePath = a_sFilePath;

	std::ifstream ifStream;
	ifStream.open(a_sFilePath.c_str(), std::ios::in | std::ios::binary);

	if (ifStream.fail())
	{
		Debug::Log("MD2 failed to load: no data detected");	
		delete this;
		return;
	}

	Header sHeader;
	ifStream.read((char*)&sHeader, sizeof(Header));

	if (sHeader.ID != MD2_MAGIC)
	{
		Debug::Log("MD2 failed to load: bad MD2 file detected");	
		delete this;
		return;
	}

	if (sHeader.Version != MD2_VERSION)
	{
		Debug::Log("MD2 failed to load: bad MD2 version detected");
		delete this;
		return;
	}
		
	RetrieveData(sHeader, ifStream);
	ifStream.close();
  
	m_Type = Type::E_MD2;
	m_bIsLoaded = true;
}

void MD2::RetrieveData(const Header& a_sHeader, std::ifstream& ifStream)
{
	/* Read File */
	char* pFrames = new char[a_sHeader.FrameSize * a_sHeader.FrameCount];
	ifStream.seekg(a_sHeader.FrameOffset, std::ios::beg);
	ifStream.read((char*)pFrames, a_sHeader.FrameSize * a_sHeader.FrameCount);

	UV* pUVs = new UV[a_sHeader.UVCount];
	ifStream.seekg(a_sHeader.UVOffset, std::ios::beg);
	ifStream.read((char*)pUVs, sizeof(UV) * a_sHeader.UVCount);
                   
	Triangle* pTriangles = new Triangle[a_sHeader.TriangleCount];
	ifStream.seekg(a_sHeader.TriangleOffset, std::ios::beg);
	ifStream.read((char*)pTriangles, sizeof(Triangle) * a_sHeader.TriangleCount);

	/* Preallocate Index Data */    
	const GLuint uiIndexCount = a_sHeader.TriangleCount * 3;
	GLushort* pIndices = new GLushort[uiIndexCount];

	/* Preallocate Vertex Data */
	const GLuint uiMaxVertexCount = uiIndexCount * a_sHeader.FrameCount;
	m_uiVertexCount = 0;
	
	GLuint uiUVSize = sizeof(glm::vec2) * uiIndexCount;
	GLuint uiPosNormSize = sizeof(glm::vec3) * uiMaxVertexCount * 2;
	GLuint uiDataSize = uiUVSize + uiPosNormSize;

	GLubyte* pVertexData = new GLubyte[uiDataSize];

	/* Required Mesh Data */
	std::unordered_map<std::string, Mesh::Animation> mAnimations;
	std::vector<Texture*> vTextures;

	/* Remap Data / Remove Duplicates */
	std::unordered_map<unsigned long long, GLuint> mHashMap;
	unsigned int* pUniqueVertexIDs = new unsigned int[uiMaxVertexCount];

	/* First Frame */
	for (GLuint i = 0; i < 1; ++i)
	{
		Frame* pFrame = (Frame*)&pFrames[i * a_sHeader.FrameSize];
		
		/* Convert From Clockwise to Counter Clockwise */
		GLuint ccwOrder[3] = { 0, 2, 1 };

		/* Each Triangle */
		for (GLuint j = 0; j < a_sHeader.TriangleCount; ++j)
		{
			/* Each Index */
			for (GLuint k = 0; k < 3; ++k)
			{
				GLuint l = ccwOrder[k];
				unsigned long long lHash;

				/* Remap Index Buffer on First Frame */
				const GLuint uiIndexID = 3 * j + k;

				const GLushort uiVertexID = pTriangles[j].Vertex[l];
				const GLushort uiNormalID = pFrame->Vertices[uiVertexID].NormalIndex;
				const GLushort uiUVID = pTriangles[j].UV[l];

				/* Create Hash Based on 3 IDs */
				lHash = uiVertexID;
				lHash <<= sizeof(GLushort) * 8;
				lHash += uiNormalID;
				lHash <<= sizeof(GLushort) * 8;
				lHash += uiUVID;

				/* Check Existance */
				std::unordered_map<unsigned long long, GLuint>::iterator it = mHashMap.find(lHash);

				if (it == mHashMap.end())
				{
					pUniqueVertexIDs[m_uiVertexCount] = uiVertexID;
					mHashMap[lHash] = mHashMap.size() - 1;

					pIndices[uiIndexID] = m_uiVertexCount;

					/* Store UV Data Once at Beginning */
					glm::vec2 v2UVs(
						pUVs[uiUVID].X / (float)a_sHeader.TextureWidth,
						pUVs[uiUVID].Y / (float)a_sHeader.TextureHeight);

					/* Set Data */
					const GLuint uiUVIndex = sizeof(glm::vec2) * m_uiVertexCount;
					memcpy(&pVertexData[uiUVIndex], &v2UVs[0], sizeof(glm::vec2));

					m_uiVertexCount++;
				}
				/* Remap */
				else
				{
					pIndices[uiIndexID] = it->second;
				}
			}
		}
	}
  
	/* Get Correct Size */
	uiUVSize = sizeof(glm::vec2) * m_uiVertexCount;  // One Frame
	uiPosNormSize = sizeof(glm::vec3) * m_uiVertexCount * a_sHeader.FrameCount * 2;     // Every Frame
	uiDataSize = uiUVSize + uiPosNormSize;

	glm::vec3 v3BoundsMin = glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX);
	glm::vec3 v3BoundsMax = glm::vec3(FLT_MIN, FLT_MIN, FLT_MIN);

	std::string sCurrentAnimation;

	/* Required for Bullet */
	glm::vec3* pPositionData = new glm::vec3[m_uiVertexCount];

	/* Each Frame */
	for (GLuint i = 0; i < a_sHeader.FrameCount; ++i)
	{
		Frame* pFrame = (Frame*)&pFrames[i * a_sHeader.FrameSize];
		
		/* Get Name */
		std::string sAnimationName = pFrame->Name;

		const size_t iIndex = sAnimationName.find_last_not_of("0123456789");
		const std::string sIndex = sAnimationName.substr(iIndex + 1, sAnimationName.size() - 1);
		
		// If animation has multiple versions also parse the version number, e.g. death101
		if(sIndex.size() >= 3)
			sAnimationName = sAnimationName.substr(0, iIndex + 2);
		
		else sAnimationName = sAnimationName.substr(0, iIndex + 1);
		
		/* Set Animation Frames */
		if (sAnimationName != sCurrentAnimation)
			mAnimations[sAnimationName].StartFrame = i;
		
		sCurrentAnimation = sAnimationName;
		mAnimations[sAnimationName].EndFrame = i;

		/* Get All Transformed Frame Vertices */
		float* pVertices[3];

		pVertices[0] = new float[a_sHeader.VertexCount];
		pVertices[1] = new float[a_sHeader.VertexCount];
		pVertices[2] = new float[a_sHeader.VertexCount];

		for (GLuint j = 0; j < a_sHeader.VertexCount; ++j)
		{
			unsigned char* cPos = pFrame->Vertices[j].Position;
			
			pVertices[j][0] = cPos[0] * pFrame->Scale.x + pFrame->Translation.x;
			pVertices[j][1] = cPos[1] * pFrame->Scale.y + pFrame->Translation.y;
			pVertices[j][2] = cPos[2] * pFrame->Scale.z + pFrame->Translation.z;
		}

		for (GLuint j = 0; j < m_uiVertexCount; ++j)
		{
			const GLuint uiIndexID = m_uiVertexCount * i + j;

			const GLushort uiVertexID = pUniqueVertexIDs[j];
			const GLushort uiNormalID = pFrame->Vertices[uiVertexID].NormalIndex;

			/* Generate Data */
			glm::vec3 v3Position(
				pVertices[uiVertexID][1],
				pVertices[uiVertexID][2],
				pVertices[uiVertexID][0]);

			/* Calculate Bounds (First Frame) */
			if (i == 0)
			{
				v3BoundsMin = glm::min(v3Position, v3BoundsMin);
				v3BoundsMax = glm::max(v3Position, v3BoundsMax);
			}

			glm::vec3 v3Normal(
				Anorms[uiNormalID][1],
				Anorms[uiNormalID][2],
				Anorms[uiNormalID][0]
			);
			
			/* Set Data to Right Offsets */
			const GLuint uiNormalOffset = sizeof(glm::vec2) * m_uiVertexCount +
				sizeof(glm::vec3) * uiIndexID
			;

			memcpy(&pVertexData[uiNormalOffset], &v3Normal[0], sizeof(glm::vec3));
	  
			const GLuint uiPositionOffset = sizeof(glm::vec2) * m_uiVertexCount +
				sizeof(glm::vec3) * m_uiVertexCount * a_sHeader.FrameCount +
				sizeof(glm::vec3) * uiIndexID
			;

			memcpy(&pVertexData[uiPositionOffset], &v3Position[0], sizeof(glm::vec3));
	  
			// First Frame Only
			if(i == 0) pPositionData[uiIndexID] = v3Position;
		}

		delete[] pVertices[0];
		delete[] pVertices[1];
		delete[] pVertices[2];
	}

	/* Generate Buffers */
	m_pVBO = new VBO(m_uiVertexCount, VertexData::Type::E_DEFAULT, pVertexData, false);

	GL_GET_ERROR();
	
	IBO* pIBO = new IBO(uiIndexCount, sizeof(GLushort), pIndices);
	pIBO->SetBounds(v3BoundsMin, v3BoundsMax);

	GL_GET_ERROR();

	/* Look for texture */
	const std::string sTexturePath = GetFolderPath(m_sFilePath) + GetFileName(m_sFilePath) + ".png";
	vTextures.push_back(
		Application::GetInstance()->GetLoadInterface()->LoadTexture(
			sTexturePath
		)
	);
  
	/* Create single mesh */
	m_vMeshes.push_back(new Mesh(m_vMeshes.size(), pIBO, mAnimations, vTextures));

	/* Cleanup */
	delete[] pVertexData;
  
	delete[] pUniqueVertexIDs;
	delete[] pFrames;
	delete[] pUVs;
	delete[] pTriangles;
}
#include "Model.h"

#include "VBO.h"
#include "IBO.h"
#include "Mesh.h"

#include "Texture.h"

#include <cfloat>

#include "Application.h"
#include "MeshRenderer.h"

Model::~Model()
{
	for (Mesh* pMesh : m_vMeshes)
	{	
		delete pMesh;
	}

	delete m_pVBO;
}

IBO* Model::GetIBO(const GLuint& a_uiID) const
{
	return m_vMeshes[a_uiID]->GetIBO();
}

const std::vector<Texture*>& Model::GetTextures(const GLuint& a_uiID) const
{
	return m_vMeshes[a_uiID]->GetTextures();
}

const std::unordered_map<std::string, Mesh::Animation>& Model::GetAnimations(const GLuint& a_uiID) const
{
	return m_vMeshes[a_uiID]->GetAnimations();
}

std::pair<GameObject*, Model*> Model::CreateObject(const std::string& a_sModelPath, const std::string& a_sVertexPath, const std::string& a_sFragmentPath)
{
	Model* pModel = Application::GetInstance()->GetLoadInterface()->LoadModel(a_sModelPath);

	if (!pModel->IsLoaded())
	{
		Debug::Log("Could not make game object because the model data is invalid");
		return std::make_pair((GameObject*)nullptr, pModel);
	}

	GameObject* pObject = GameObject::Create();

	/* Split model into multiple mesh gameobjects */
	if (pModel->GetMeshes().size() > 1)
	{
		for (int i = 0; i < pModel->GetMeshes().size(); ++i)
		{
			GameObject* pMesh = GameObject::Create();
			MeshRenderer* pMeshRenderer = new MeshRenderer(
				pObject,
				a_sModelPath,
				i,
				nullptr,
				a_sVertexPath,
				a_sFragmentPath
			);

			pMesh->AddComponent(pMeshRenderer);
			pMesh->SetParent(pObject);
		}
	}
	/* Assign model mesh to gameobject */
	else
	{
		MeshRenderer* pMeshRenderer = new MeshRenderer(
			pObject,
			a_sModelPath,
			0,
			nullptr,
			a_sVertexPath,
			a_sFragmentPath
		);

		pObject->AddComponent(pMeshRenderer);
	}

	/* Delete reference once because we used it for reading */
	pModel->DelReference();

	return std::make_pair(pObject, pModel);
}

void Model::CalculateBounds()
{
	m_v3BoundsMin = glm::vec3(1.0f) * FLT_MAX;
	m_v3BoundsMax = glm::vec3(1.0f) * FLT_MIN;

	for (Mesh* pMesh : m_vMeshes)
	{
		IBO* pIBO = pMesh->GetIBO();
		
		m_v3BoundsMin = glm::min(m_v3BoundsMin, pIBO->GetBoundsMin());
		m_v3BoundsMax = glm::max(m_v3BoundsMax, pIBO->GetBoundsMax());
	}

	m_v3Offset = (m_v3BoundsMax + m_v3BoundsMin) * 0.5f;
	m_v3Extents = (m_v3BoundsMax - m_v3BoundsMin) * 0.5f;

	m_fRadius = glm::distance(glm::vec3(), m_v3Extents);
}

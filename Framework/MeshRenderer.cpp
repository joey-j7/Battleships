#include "MeshRenderer.h"
#include "Utils.h"
#include "Application.h"
#include "ShaderProgram.h"

#include "VBO.h"
#include "IBO.h"

#include "Scene.h"
#include "Camera.h"
#include "RenderSystem.h"

MeshRenderer::MeshRenderer(
	GameObject* a_pGameObject,
	const std::string& a_sModelPath,
	unsigned int a_uiMeshIndex,
	Texture* a_pTexture,
	const std::string& a_sVertPath,
	const std::string& a_sFragPath
) : 
	Renderer(
		a_pGameObject,
		Application::GetInstance()->GetLoadInterface()->LoadShader(
			a_sVertPath,
			a_sFragPath
		)
	)
{
	m_pGameObject = a_pGameObject;
	m_pModel = Application::GetInstance()->GetLoadInterface()->LoadModel(a_sModelPath);

	m_uiMeshIndex = a_uiMeshIndex;

	m_pIBO = m_pModel->GetIBO(m_uiMeshIndex);
	m_pVBO = m_pModel->GetVBO();

	/* Add texture */
	const std::vector<Texture*>& vTextures = GetMesh()->GetTextures();

	if (vTextures.size() > 1)
		m_pTextureLQ = GetMesh()->GetTextures()[1];

	if (a_pTexture)
		m_pTexture = a_pTexture;
	else
	{
		if (!vTextures.empty())
			m_pTexture = vTextures.front();
	}

  SetType(E_Render);
}

MeshRenderer::MeshRenderer(
	GameObject* a_pGameObject,
	Shape::Type a_eType,
	unsigned int a_uiSegments,
	Texture* a_pTexture,
	const std::string& a_sVertPath,
	const std::string& a_sFragPath
) : 
	Renderer(
		a_pGameObject,
		Application::GetInstance()->GetLoadInterface()->LoadShader(
			a_sVertPath,
			a_sFragPath
		)
	)
{
	m_pGameObject = a_pGameObject;
	m_pShape = Application::GetInstance()->GetLoadInterface()->LoadShape(a_eType, a_uiSegments);

	m_pIBO = m_pShape->GetIBO();
	m_pVBO = m_pShape->GetVBO();

	/* Add texture */
	if (a_pTexture)
		m_pTexture = a_pTexture;
	else
	{
		if (GetMesh()->GetTextures().size() > 1)
			m_pTextureLQ = GetMesh()->GetTextures()[1];

		const std::vector<Texture*>& vTextures = GetMesh()->GetTextures();
		if (!vTextures.empty())
			m_pTexture = vTextures.front();
	}

  m_eType = E_Render;
}

MeshRenderer::~MeshRenderer()
{
	if (m_pShape)
		Application::GetInstance()->GetLoadInterface()->DeleteShape(m_pShape->GetType(), m_pShape->GetSegmentCount());
}

bool MeshRenderer::Render(Camera* a_pCamera)
{
	m_bVisible = m_fOpacity > 0;

	/* Check for model visibility */
	if (m_bVisible)
	{
		if (m_pModel)
			m_bVisible = a_pCamera->IsVisible(m_pGameObject->GetTransform(), GetMesh());
	}

	if (!m_bVisible)
		return false;

	/* Texture LOD */
	const float fLODDistance = 75;
	const float fCamDistance = std::abs(
		a_pCamera->GetTransform()->GetWorldPosition().y -
		m_pGameObject->GetTransform()->GetWorldPosition().y
	);

	const bool bSwapTexDetail = m_pTextureLQ && fCamDistance > fLODDistance;

	if (bSwapTexDetail)
	{
		m_pTextureLQ->Bind(0);
	}

	/* Enable attributes and uniforms */
	for (auto& pUniform : m_pShader->GetUniforms())
	{
    if (pUniform.first == "u_model")
    {
      glm::mat4 m4Matrix = m_pGameObject->GetTransform()->GetTransformMatrix();
      glUniformMatrix4fv(pUniform.second.ID, 1, GL_FALSE, &m4Matrix[0][0]);
      GL_GET_ERROR();
    }
    else if (pUniform.first == "u_mvp")
    {
      glm::mat4 m4Matrix = a_pCamera->GetProjectionViewMatrix() * m_pGameObject->GetTransform()->GetTransformMatrix();
      glUniformMatrix4fv(pUniform.second.ID, 1, GL_FALSE, &m4Matrix[0][0]);
      GL_GET_ERROR();
    }
    else if (pUniform.first == "u_lightPosition")
    {
      const glm::vec3& v3Position = RenderSystem::LightObject->GetTransform()->GetWorldPosition();
      glUniform3fv(pUniform.second.ID, 1, &v3Position[0]);
      GL_GET_ERROR();
    }
    else if (pUniform.first == "u_camPos")
    {
      glm::vec3 v3Position = a_pCamera->GetTransform()->GetWorldPosition();
      glUniform3fv(pUniform.second.ID, 1, &v3Position[0]);
      GL_GET_ERROR();
    }
    else if (pUniform.first == "u_opacity")
    {
      glUniform1f(pUniform.second.ID, m_fOpacity);
      GL_GET_ERROR();
    }
    else if (pUniform.first == "s_texture")
    {
      glUniform1i(pUniform.second.ID, 0);
      GL_GET_ERROR();
    }
    /* Already set in RenderSystem */
    else if (
      pUniform.first == "u_projection" ||
      pUniform.first == "u_view" ||
      pUniform.first == "u_vp" ||
      pUniform.first == "u_pv"
    )
      continue;
		else
			Debug::Log("Warning! Missing MeshRenderer uniform: " + pUniform.first);
	}

	/* Draw */
	glDrawElements(GL_TRIANGLES, GetIBO()->GetCount(), GL_UNSIGNED_SHORT, 0);
	GL_GET_ERROR();
  
	/* Revert to HQ */
	if (bSwapTexDetail)
		m_pTexture->Bind(0);

	return true;
}

VBO* MeshRenderer::GetVBO() const
{
	return m_pVBO;
}

IBO* MeshRenderer::GetIBO() const
{
	return m_pIBO;
}

#include "SkyboxRenderer.h"
#include "Application.h"
#include "Utils.h"
#include "ShaderProgram.h"
#include "Camera.h"
#include "SceneManager.h"
#include "Scene.h"

#include "VBO.h"
#include "IBO.h"

SkyboxRenderer::SkyboxRenderer(GameObject* a_pGameObject, Texture* a_pTexture) :
	Renderer(
		a_pGameObject,
		Application::GetInstance()->GetLoadInterface()->LoadShader(
			"Assets/Engine/Shaders/Skybox/Skybox3D.vsh",
			"Assets/Engine/Shaders/Skybox/Skybox3D.fsh"
		)
	)
{
	m_pGameObject = a_pGameObject;
	m_pTexture = a_pTexture;
	m_pModel = Application::GetInstance()->GetLoadInterface()->LoadModel("Assets/Engine/Models/Box/BoxHalf.obj");
	m_pCamera = Application::GetInstance()->GetSceneManager()->GetActiveScene()->GetCamera();

	SetType(E_RenderSkybox);
}

SkyboxRenderer::~SkyboxRenderer()
{
	
}

bool SkyboxRenderer::Render(Camera* a_pCamera)
{
	if (m_fOpacity == 0.0f)
		return false;

	glDisable(GL_CULL_FACE);

	m_pShader->Activate();
	GL_GET_ERROR();

	m_pModel->GetVBO()->Bind();
	m_pModel->GetIBO(0)->Bind();
	GL_GET_ERROR();

	for (const auto& pUniforms : m_pShader->GetUniforms())
	{
		if (pUniforms.first == "u_model")
		{
			const glm::mat4 m4Matrix = m_pGameObject->GetTransform()->GetTransformMatrix();
			glUniformMatrix4fv(pUniforms.second.ID, 1, GL_FALSE, &m4Matrix[0][0]);
			GL_GET_ERROR();
		}
		else if (pUniforms.first == "u_view")
		{
			glm::mat4 m4Matrix = a_pCamera->GetViewMatrix();
			m4Matrix[3][0] = 0;
			m4Matrix[3][1] = 0;
			m4Matrix[3][2] = 0;

			glUniformMatrix4fv(pUniforms.second.ID, 1, GL_FALSE, &m4Matrix[0][0]);
			GL_GET_ERROR();
		}
		else if (pUniforms.first == "u_pv")
		{
			glm::mat4 m4View = a_pCamera->GetViewMatrix();
			m4View[3][0] = 0;
			m4View[3][1] = 0;
			m4View[3][2] = 0;

			glm::mat4 m4Matrix = a_pCamera->GetProjectionMatrix() * m4View;

			glUniformMatrix4fv(pUniforms.second.ID, 1, GL_FALSE, &m4Matrix[0][0]);
			GL_GET_ERROR();
		}
		else if (pUniforms.first == "u_opacity")
		{
			glUniform1f(pUniforms.second.ID, m_fOpacity);
			GL_GET_ERROR();
		}
		else if (pUniforms.first == "s_texture")
		{
			if (GetTexture() != nullptr)
			{
				GetTexture()->Bind(0);
				glUniform1i(pUniforms.second.ID, 0);
				GL_GET_ERROR();
			}
		}
    /* Already set in RenderSystem */
    else if (
      pUniforms.first == "u_projection" ||
      pUniforms.first == "u_view" ||
      pUniforms.first == "u_vp" ||
      pUniforms.first == "u_pv"
      )
      continue;
		else
			Debug::Log("Warning! Missing SkyboxRenderer uniform: " + pUniforms.first);
	}

	/* Draw */
	glDrawElements(GL_TRIANGLES, m_pModel->GetIBO(0)->GetCount(), GL_UNSIGNED_SHORT, 0);
	GL_GET_ERROR();
  
	glEnable(GL_CULL_FACE);
	return true;
}

VBO* SkyboxRenderer::GetVBO() const
{
	return m_pModel->GetVBO();
}

IBO* SkyboxRenderer::GetIBO() const
{
	return m_pModel->GetIBO(0);
}

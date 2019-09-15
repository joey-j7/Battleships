#include "SpriteRenderer.h"
#include "Utils.h"
#include "Application.h"
#include "ShaderProgram.h"

#include "Shape.h"

#include "VBO.h"
#include "IBO.h"

#include "Scene.h"
#include "Camera.h"

SpriteRenderer::SpriteRenderer(GameObject* a_pGameObject, const std::string& a_sTexturePath, bool a_bScreenToWorld, bool a_bScaleResolution)
	: Renderer(
		a_pGameObject,
		Application::GetInstance()->GetLoadInterface()->LoadShader(
			"Assets/Engine/Shaders/Default/Unlit2D.vsh",
			"Assets/Engine/Shaders/Default/Unlit2D.fsh"
	)
)
{
	m_pGameObject = a_pGameObject;
	m_pShape = Application::GetInstance()->GetLoadInterface()->LoadShape(Shape::E_QUAD, 1);
	m_pTexture = Application::GetInstance()->GetLoadInterface()->LoadTexture(a_sTexturePath);

	m_bScreenToWorld = a_bScreenToWorld;
	m_bScaleResolution = a_bScaleResolution;

	SetType(E_Render2D);
}

SpriteRenderer::~SpriteRenderer()
{
	Application::GetInstance()->GetLoadInterface()->DeleteShape(m_pShape->GetType(), m_pShape->GetSegmentCount());
}

bool SpriteRenderer::Render(Camera* a_pCamera)
{
	m_bVisible = m_fOpacity > 0;

	if (!m_bVisible)
		return false;

	// Get Model Matrix
	glm::mat4 m4ModelMatrix = CreateTransformMatrix(a_pCamera);

	if (!m_bVisible)
		return false;

	for (auto& pUniform : m_pShader->GetUniforms())
	{
		if (pUniform.first == "u_model")
		{
			glUniformMatrix4fv(pUniform.second.ID, 1, GL_FALSE, &m4ModelMatrix[0][0]);
			GL_GET_ERROR();
		}
		else if (pUniform.first == "u_orthoModel")
		{
			glm::mat4 m4Matrix = Camera::GetOrthoMatrix() * m4ModelMatrix;	
			glUniformMatrix4fv(pUniform.second.ID, 1, GL_FALSE, &m4Matrix[0][0]);
			GL_GET_ERROR();
		}
		else if (pUniform.first == "u_mvp")
		{
			glm::mat4 m4Matrix = a_pCamera->GetProjectionViewMatrix() * m4ModelMatrix;	
			glUniformMatrix4fv(pUniform.second.ID, 1, GL_FALSE, &m4Matrix[0][0]);
			GL_GET_ERROR();
		}
		else if (pUniform.first == "u_opacity")
		{
			glUniform1f(pUniform.second.ID, m_fOpacity);
			GL_GET_ERROR();
		}
		else if (pUniform.first == "u_color")
		{
			glUniform3fv(pUniform.second.ID, 1, &m_v3Color[0]);
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
			pUniform.first == "u_pv") continue;
		else
			Debug::Log("Warning! Missing SpriteRenderer uniform: " + pUniform.first);
	}

	/* Draw */
	glDrawElements(GL_TRIANGLES, GetIBO()->GetCount(), GL_UNSIGNED_SHORT, 0);
	GL_GET_ERROR();

	return true;
}

VBO* SpriteRenderer::GetVBO() const
{
	return m_pShape->GetVBO();
}

IBO* SpriteRenderer::GetIBO() const
{
	return m_pShape->GetIBO();
}

glm::mat4 SpriteRenderer::CreateTransformMatrix(Camera* a_pCamera)
{
	Transform* pTransform = m_pGameObject->GetTransform();

	glm::vec3 v3Position;
	glm::vec3 v3Scale = pTransform->GetScale() * m_fScale;

	if (m_bScreenToWorld)
	{
		v3Position = glm::vec3(
			a_pCamera->ConvertWorldPositionToScreen(
				pTransform->GetWorldPosition()
			),
			0.0f);

		/* When screen position is invalid */
		if (v3Position.x == FLT_MIN)
		{
			m_bVisible = false;
			return glm::mat4();
		}
	}
	else {
		v3Position = pTransform->GetWorldPosition();

		if (m_bScaleResolution)
		{
			v3Position.x *= GLWindow::GetResolutionScale().x;
			v3Position.y *= GLWindow::GetResolutionScale().y;
		}
	}
	
	v3Scale *= GLWindow::GetScale();

	const glm::vec3 v3Offset = m_bCenterOrigin ? glm::vec3() : glm::vec3(
		m_pTexture->GetWidth() * v3Scale.x,
		m_pTexture->GetHeight() * v3Scale.y,
		0
	);

	glm::mat4 transform = glm::translate(v3Position + v3Offset * 0.5f);
	transform *= glm::scale(v3Scale);

	if (!m_bCenterOrigin)
	{
		transform[0][0] = v3Offset.x;
		transform[1][1] = v3Offset.y;
	}

	return transform;
}

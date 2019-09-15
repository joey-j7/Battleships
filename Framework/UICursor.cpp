#include "UICursor.h"

#include "Application.h"

#include "Transform.h"
#include "ShaderProgram.h"

#include "Utils.h"
#include "VertexStructures.h"

#include "Camera.h"

#include "VBO.h"
#include "IBO.h"

UICursor::UICursor(const std::string& a_sFilePath)
{
#ifndef ANDROID
	m_pInput = Application::GetInstance()->GetInput();

	m_v3Scale = glm::vec3(0.5f, 0.5f, 0.5f);

	m_pShape = Application::GetInstance()->GetLoadInterface()->LoadShape(Shape::E_QUAD, 1);
	m_pShader = Application::GetInstance()->GetLoadInterface()->LoadShader("Assets/Engine/Shaders/Default/Unlit2D.vsh", "Assets/Engine/Shaders/Default/Unlit2D.fsh");

	m_pTexture = Application::GetInstance()->GetLoadInterface()->LoadTexture(a_sFilePath);
#endif
}

void UICursor::Update()
{
#ifndef ANDROID
	if (!m_bActive) return;
	m_v3Position = glm::vec3(m_pInput->GetMousePosition(), 0);
#endif
}

void UICursor::Draw() const
{
#ifndef ANDROID
	if (!m_bActive) return;

	Bind();

	// Get Model Matrix
	glm::mat4 m4ModelMatrix = CreateTransformMatrix();

	for (const auto& pUniforms : m_pShader->GetUniforms())
	{
		if (pUniforms.first == "u_ortho")
		{
			const glm::mat4 m4Matrix = Camera::GetOrthoMatrix();	
			glUniformMatrix4fv(pUniforms.second.ID, 1, GL_FALSE, &m4Matrix[0][0]);
			GL_GET_ERROR();
		}
		else if (pUniforms.first == "u_model")
		{
			glUniformMatrix4fv(pUniforms.second.ID, 1, GL_FALSE, &m4ModelMatrix[0][0]);
			GL_GET_ERROR();
		}
		else if (pUniforms.first == "u_orthoModel")
		{
			const glm::mat4 m4Matrix = Camera::GetOrthoMatrix() * m4ModelMatrix;	
			glUniformMatrix4fv(pUniforms.second.ID, 1, GL_FALSE, &m4Matrix[0][0]);
			GL_GET_ERROR();
		}
		else if (pUniforms.first == "u_opacity")
		{
			glUniform1f(pUniforms.second.ID, 1.0f);
			GL_GET_ERROR();
		}
		else if (pUniforms.first == "u_color")
		{
			glm::vec3 v3Color = glm::vec3(1.f, 1.f, 1.f);
			glUniform3fv(pUniforms.second.ID, 1, &v3Color[0]);
			GL_GET_ERROR();
		}
		else if (pUniforms.first == "s_texture")
		{
			if (m_pTexture)
			{
				m_pTexture->Bind(0);
				glUniform1i(pUniforms.second.ID, 0);
				GL_GET_ERROR();
			}
		}
    else
      Debug::Log("Warning! Missing UICursor uniform: " + pUniforms.first);
	}

	glDrawElements(GL_TRIANGLES, m_pShape->GetIBO()->GetCount(), GL_UNSIGNED_SHORT, 0);
	GL_GET_ERROR();

	Unbind();
#endif
}

void UICursor::Bind() const
{
#ifndef ANDROID
	m_pShader->Activate();
	m_pShape->GetVBO()->Bind();

	const size_t szSize = VertexData::GetSize(VertexData::E_DEFAULT);
	const std::vector<int> vOffsets = VertexData::GetOffsets(VertexData::E_DEFAULT);

	for (auto& pAttribute : m_pShader->GetAttributes())
	{
		glEnableVertexAttribArray(pAttribute.second.ID);

		if (pAttribute.first == "a_position")
			glVertexAttribPointer(pAttribute.second.ID, 3, GL_FLOAT, GL_FALSE, szSize, (GLvoid*)vOffsets[0]);
		else if (pAttribute.first == "a_normal")
			glVertexAttribPointer(pAttribute.second.ID, 3, GL_FLOAT, GL_FALSE, szSize, (GLvoid*)vOffsets[1]);
		else if (pAttribute.first == "a_uv")
			glVertexAttribPointer(pAttribute.second.ID, 2, GL_FLOAT, GL_FALSE, szSize, (GLvoid*)vOffsets[2]);
    else
      Debug::Log("Warning! Missing UICursor attribute: " + pAttribute.first);
	}

	m_pShape->GetIBO()->Bind();

	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#endif
}

void UICursor::Unbind() const
{
#ifndef ANDROID
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

  m_pShader->DisableAttributes();
  ShaderProgram::Deactivate();

  if (m_pTexture) m_pTexture->Unbind();
	VBO::Unbind();
	IBO::Unbind();

	GL_GET_ERROR();
#endif
}

glm::mat4 UICursor::CreateTransformMatrix() const
{
#ifndef ANDROID
	const glm::vec2 v2Scale = m_v3Scale * glm::min(GLWindow::GetDPIScale().x, GLWindow::GetDPIScale().y) * GLWindow::GetScale();

	const glm::vec3 v3Offset = glm::vec3(m_pTexture->GetWidth() * v2Scale.x, m_pTexture->GetHeight() * v2Scale.y, 0);
	glm::mat4 transform = glm::translate(m_v3Position + v3Offset * 0.5f);
	transform *= glm::scale(glm::vec3(v2Scale, 1.0f));
	transform[0][0] = v3Offset.x;
	transform[1][1] = v3Offset.y;

	return transform;
#endif
}

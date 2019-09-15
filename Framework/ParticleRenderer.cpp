#include "ParticleRenderer.h"

#include "Application.h"

#include "Camera.h"
#include "VBO.h"
#include "Utils.h"
#include "VertexStructures.h"
#include "GameTime.h"
#include <cmath>

ParticleRenderer::ParticleRenderer(GameObject* a_pGameObject, const Data& a_Data, const std::string& a_sFilePath)
	: Renderer(
		a_pGameObject,
		Application::GetInstance()->GetLoadInterface()->LoadShader(
			"Assets/Engine/Shaders/Particles/Default.vsh",
			a_sFilePath == "" ? "Assets/Engine/Shaders/Particles/Textureless.fsh" : "Assets/Engine/Shaders/Particles/Default.fsh"
		)
)
{
	m_Data = a_Data;
	m_Data.Count = std::min(m_Data.Count, (unsigned int)m_iMaxParticles);

	m_fTime = m_Data.TimeOffset;

	std::vector<VertexData::Particle> vParticles;
	vParticles.resize(m_Data.Count);

	m_v3Color = m_Data.Color;

	const float fInvCount = 1.0f / (float)m_Data.Count;
	m_fInvLifeTime = 1.0f / m_Data.LifeTime;
		
	for (unsigned int i = 0; i < m_Data.Count; ++i)
	{
		// Assign a unique ID to each particle, between 0 and 360 (in radians)
		vParticles[i].ID = glm::radians((i * fInvCount) * m_Data.Degrees);
		
		// Assign random offsets within bounds
		vParticles[i].RadiusOffset = Random::InRange(m_Data.Radius, 1.00f);
		vParticles[i].VelocityOffset = Random::InRange(0.0f, m_Data.Velocity);
		vParticles[i].SizeOffset = Random::InRange(0.0f, m_Data.Size * 0.5f);
	}

	/* Assign texture */
	if (!a_sFilePath.empty())
		m_pTexture = Application::GetInstance()->GetLoadInterface()->LoadTexture(a_sFilePath);

	m_pBuffer = new VBO(m_Data.Count, VertexData::Type::E_PARTICLE, vParticles.data());
  SetType(E_RenderParticles);
}

ParticleRenderer::~ParticleRenderer()
{
	delete m_pBuffer;
}

bool ParticleRenderer::Render(Camera* a_pCamera)
{
	m_bVisible = m_fOpacity > 0.02f && a_pCamera->IsInFrustum(GetObject()->GetTransform()->GetPosition(), 10.0f);

	if (!m_bVisible)
		return false;

	glDisable(GL_CULL_FACE);

	for (const auto& pUniforms : m_pShader->GetUniforms())
	{
		if (pUniforms.first == "u_mvp")
		{
			glm::mat4 m4Matrix = a_pCamera->GetProjectionViewMatrix() * GetObject()->GetTransform()->GetTransformMatrix();
			glUniformMatrix4fv(pUniforms.second.ID, 1, GL_FALSE, &m4Matrix[0][0]);
			GL_GET_ERROR();
		}
		else if (pUniforms.first == "u_time")
		{
			m_fTime = std::fmod(m_fTime + GameTime::GetDeltaTime(), m_Data.LifeTime);

			glUniform1f(pUniforms.second.ID, m_fTime);
			GL_GET_ERROR();
		}
		else if (pUniforms.first == "u_radius")
		{
			glUniform1f(pUniforms.second.ID, m_Data.Radius);
			GL_GET_ERROR();
		}
		else if (pUniforms.first == "u_velocity")
		{
			glUniform1f(pUniforms.second.ID, m_Data.Velocity);
			GL_GET_ERROR();
		}
		else if (pUniforms.first == "u_size")
		{
			glUniform1f(pUniforms.second.ID, m_Data.Size);
			GL_GET_ERROR();
		}
		else if (pUniforms.first == "u_camDist")
		{
			const glm::vec3 v3CamPos = a_pCamera->GetTransform()->GetWorldPosition();
			const glm::vec3 v3Pos = GetObject()->GetTransform()->GetWorldPosition();
			const float fDistance = glm::max(glm::distance(v3CamPos, v3Pos), 0.001f);

			glUniform1f(pUniforms.second.ID, fDistance);
			GL_GET_ERROR();
		}
		else if (pUniforms.first == "u_color")
		{
			glUniform3fv(pUniforms.second.ID, 1, &m_v3Color[0]);
			GL_GET_ERROR();
		}
		else if (pUniforms.first == "u_offset")
		{
			glUniform3fv(pUniforms.second.ID, 1, &m_Data.Offset[0]);
			GL_GET_ERROR();
		}
		else if (pUniforms.first == "u_opacity")
		{
			float fNormTime = m_fTime * m_fInvLifeTime;
			fNormTime *= 2.0f;

			if (fNormTime > 1.0f)
				fNormTime = 2.0f - fNormTime;
      
			glUniform1f(pUniforms.second.ID, m_fOpacity * fNormTime);
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
    /* Already set in RenderSystem */
    else if (
      pUniforms.first == "u_projection" ||
      pUniforms.first == "u_view" ||
      pUniforms.first == "u_vp" ||
      pUniforms.first == "u_pv"
    )
      continue;
		else
			Debug::Log("Warning! Missing ParticleRenderer uniform: " + pUniforms.first);
	}

	/* Draw */
	glDrawArrays(GL_POINTS, 0, m_Data.Count);
	GL_GET_ERROR();

	glEnable(GL_CULL_FACE);

	return true;
}

VBO* ParticleRenderer::GetVBO() const
{
	return m_pBuffer;
}

IBO* ParticleRenderer::GetIBO() const
{
	return nullptr;
}

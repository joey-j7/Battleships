#include "WaterRenderer.h"

#include "ShaderProgram.h"

#include "SceneManager.h"
#include "Scene.h"

#include "Camera.h"
#include "GameObject.h"

#include "VBO.h"
#include "IBO.h"

#include "Application.h"
#include "GameTime.h"
#include "VertexStructures.h"
#include "Utils.h"

const std::string WaterRenderer::m_vCubeTexturesLQ[6] = {
	"Assets/Engine/Textures/SkyLQ/right.jpg",
	"Assets/Engine/Textures/SkyLQ/left.jpg",
	"Assets/Engine/Textures/SkyLQ/top.jpg",
	"Assets/Engine/Textures/SkyLQ/bottom.jpg",
	"Assets/Engine/Textures/SkyLQ/back.jpg",
	"Assets/Engine/Textures/SkyLQ/front.jpg"
};

WaterRenderer::WaterRenderer(GameObject* a_pGameObject)
	: Renderer(
		a_pGameObject,
		Application::GetInstance()->GetLoadInterface()->LoadShader(
			"Assets/Engine/Shaders/Water/water.vsh",
			"Assets/Engine/Shaders/Water/water.fsh"
		)
	)
{
	m_pGameObject = a_pGameObject;
	m_pTexture = Application::GetInstance()->GetLoadInterface()->LoadCubeTexture(m_vCubeTexturesLQ);

	/* Set DUDV Offset Texture */
	m_pDUDVTexture = Application::GetInstance()->GetLoadInterface()->LoadTexture("Assets/Engine/Textures/Water/dudv.jpg");

	m_pCamera = Application::GetInstance()->GetSceneManager()->GetActiveScene()->GetCamera();
	
	m_pQuad = Application::GetInstance()->GetLoadInterface()->LoadShape(Shape::E_QUAD, 1);
	SetType(E_RenderWater);
}

WaterRenderer::~WaterRenderer()
{
	Application::GetInstance()->GetLoadInterface()->DeleteTexture(m_pDUDVTexture->GetPath());
	Application::GetInstance()->GetLoadInterface()->DeleteShape(m_pQuad->GetType(), m_pQuad->GetSegmentCount());

	m_pShader = nullptr;
}

bool WaterRenderer::Render(Camera* a_pCamera)
{
	if (m_fOpacity == 0.0f)
		return false;

	/* Set quality based on camera distance */
	const float fCamY = a_pCamera->GetTransform()->GetWorldPosition().y;
	const float fObjY = GetObject()->GetTransform()->GetWorldPosition().y;
	m_fHeightDistance = fCamY - fObjY;

	m_pShader->Activate();

	/* Enable Attributes */
  const size_t szSize = VertexData::GetSize(VertexData::E_DEFAULT);
  const std::vector<int> vOffsets = VertexData::GetOffsets(VertexData::E_DEFAULT);

  for (auto& pAttribute : m_pShader->GetAttributes())
  {
    if (pAttribute.first == "a_position")
      glVertexAttribPointer(pAttribute.second.ID, 3, GL_FLOAT, GL_FALSE, szSize, (GLvoid*)vOffsets[0]);
    else if (pAttribute.first == "a_normal")
      glVertexAttribPointer(pAttribute.second.ID, 3, GL_FLOAT, GL_FALSE, szSize, (GLvoid*)vOffsets[1]);
    else if (pAttribute.first == "a_uv")
      glVertexAttribPointer(pAttribute.second.ID, 2, GL_FLOAT, GL_FALSE, szSize, (GLvoid*)vOffsets[2]);
    else
      Debug::Log("Warning! Missing WaterRenderer attribute: " + pAttribute.first);
  }

	/* Enable Uniforms */
  for (const auto& pUniforms : m_pShader->GetUniforms())
  {
    if (pUniforms.first == "u_projection")
    {
    }
    else if (pUniforms.first == "u_view")
    {
    }
    else if (pUniforms.first == "u_model")
    {
      const glm::mat4 m4Matrix = m_pGameObject->GetTransform()->GetTransformMatrix();
      glUniformMatrix4fv(pUniforms.second.ID, 1, GL_FALSE, &m4Matrix[0][0]);
      GL_GET_ERROR();
    }
    else if (pUniforms.first == "u_pv")
    {
    }
    else if (pUniforms.first == "u_mvp")
    {
      const glm::mat4 m4Matrix = m_pCamera->GetProjectionViewMatrix() * m_pGameObject->GetTransform()->GetTransformMatrix();
      glUniformMatrix4fv(pUniforms.second.ID, 1, GL_FALSE, &m4Matrix[0][0]);
      GL_GET_ERROR();
    }
    else if (pUniforms.first == "u_heightDistance")
    {
      glUniform1f(pUniforms.second.ID, m_fHeightDistance);
      GL_GET_ERROR();
    }
    else if (pUniforms.first == "u_waveOffset")
    {
      m_v2WaveOffset.x += m_v2WaveSpeed.x * GameTime::GetDeltaTime();
      m_v2WaveOffset.x = std::fmod(m_v2WaveOffset.x, 1);
      m_v2WaveOffset.y = sin(m_v2WaveOffset.x * igad::PI * m_v2WaveSpeed.y) * m_fWaveHeight + m_fWaveHeight;

      glUniform2fv(pUniforms.second.ID, 1, &m_v2WaveOffset[0]);
      GL_GET_ERROR();
    }
    else if (pUniforms.first == "u_waterColor")
    {
      glUniform4fv(pUniforms.second.ID, 1, &m_v4Color[0]);
      GL_GET_ERROR();
    }
    else if (pUniforms.first == "u_camPos")
    {
      const glm::vec3& v3CamPos = m_pCamera->GetTransform()->GetWorldPosition();
      glUniform3fv(pUniforms.second.ID, 1, &v3CamPos[0]);
      GL_GET_ERROR();
    }
    else if (pUniforms.first == "u_opacity")
    {
      glUniform1f(pUniforms.second.ID, m_fOpacity);
      GL_GET_ERROR();
    }
    else if (pUniforms.first == "s_skyTexture")
    {
      if (m_pTexture)
      {
        m_pTexture->Bind(0);
        glUniform1i(pUniforms.second.ID, 0);
        GL_GET_ERROR();
      }
    }
    else if (pUniforms.first == "s_dudvTexture")
    {
      if (m_pDUDVTexture)
      {
        m_pDUDVTexture->Bind(1);
        glUniform1i(pUniforms.second.ID, 1);
        GL_GET_ERROR();
      }
    }
    else
      Debug::Log("Warning! Missing WaterRenderer uniform: " + pUniforms.first);
  }

  /* Draw */
	glDrawElements(GL_TRIANGLES, m_pQuad->GetIBO()->GetCount(), GL_UNSIGNED_SHORT, 0);
	GL_GET_ERROR();

	return true;
}

VBO* WaterRenderer::GetVBO() const
{
	return m_pQuad->GetVBO();
}

IBO* WaterRenderer::GetIBO() const
{
	return m_pQuad->GetIBO();
}
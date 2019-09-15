#include "RenderSystem.h"
#include "Transform.h"
#include "Utils.h"
#include "Application.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Camera.h"
#include "Renderer.h"
#include "PhysicsDebugDrawer.h"

#include "VBO.h"
#include "IBO.h"
#include "ShaderProgram.h"
#include "Texture.h"

GameObject* RenderSystem::LightObject = nullptr;

RenderSystem::RenderSystem()
{
	if (!LightObject)
	{
		LightObject = GameObject::Create();
		LightObject->SetTag("Light Position");
	}

	Scene* pScene = Application::GetInstance()->GetSceneManager()->GetActiveScene();
	m_pCamera = pScene->GetCamera();
	m_pPhysicsDebugDrawer = PhysicsDebugDrawer::GetInstance();

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

RenderSystem::~RenderSystem()
{
	LightObject = nullptr;
}

bool RenderSystem::CanProcessComponent(Component* a_pComponent)
{
	return dynamic_cast<Renderer*>(a_pComponent);
}

void RenderSystem::ProcessComponents()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for (auto& mType : m_vRenderBatches)
	{
		const bool bIs2D = mType.first == Component::E_Render2D || mType.first == Component::E_Render2DAfter || mType.first == Component::E_RenderLoaders;

		if (bIs2D)
		{
			glDisable(GL_CULL_FACE);
			glDisable(GL_DEPTH_TEST);
		}

		for (auto& mVBO : mType.second)
		{
			mVBO.first->Bind();
			GL_GET_ERROR();

			for (auto& mShader : mVBO.second)
			{
				mShader.first->Activate();
				GL_GET_ERROR();

				/* Enable attributes */
				VertexData::Type eType = mType.first == Component::E_RenderParticles ? VertexData::E_PARTICLE : VertexData::E_DEFAULT;

				const size_t szSize = VertexData::GetSize(eType);
				const std::vector<int> vOffsets = VertexData::GetOffsets(eType);

				for (auto& pAttribute : mShader.first->GetAttributes())
				{
					glEnableVertexAttribArray(pAttribute.second.ID);

					if (pAttribute.first == "a_position")
						glVertexAttribPointer(pAttribute.second.ID, 3, GL_FLOAT, GL_FALSE, szSize, (GLvoid*)vOffsets[0]);
					else if (pAttribute.first == "a_normal")
						glVertexAttribPointer(pAttribute.second.ID, 3, GL_FLOAT, GL_FALSE, szSize, (GLvoid*)vOffsets[1]);
					else if (pAttribute.first == "a_uv")
						glVertexAttribPointer(pAttribute.second.ID, 2, GL_FLOAT, GL_FALSE, szSize, (GLvoid*)vOffsets[2]);

					else if (pAttribute.first == "a_id")
						glVertexAttribPointer(pAttribute.second.ID, 1, GL_FLOAT, GL_FALSE, szSize, (GLvoid*)vOffsets[0]);
					else if (pAttribute.first == "a_radiusOffset")
						glVertexAttribPointer(pAttribute.second.ID, 1, GL_FLOAT, GL_FALSE, szSize, (GLvoid*)vOffsets[1]);
					else if (pAttribute.first == "a_velocityOffset")
						glVertexAttribPointer(pAttribute.second.ID, 1, GL_FLOAT, GL_FALSE, szSize, (GLvoid*)vOffsets[2]);
					else if (pAttribute.first == "a_sizeOffset")
						glVertexAttribPointer(pAttribute.second.ID, 1, GL_FLOAT, GL_FALSE, szSize, (GLvoid*)vOffsets[3]);
				}

				auto& mUniforms = mShader.first->GetUniforms();

				/* Set known camera data */
				for (auto& uniform : mUniforms)
				{
					if (uniform.first == "u_projection")
					{
						const glm::mat4 m4Matrix = m_pCamera->GetProjectionMatrix();	
						glUniformMatrix4fv(uniform.second.ID, 1, GL_FALSE, &m4Matrix[0][0]);
						GL_GET_ERROR();
					}
					else if (uniform.first == "u_view")
					{
						const glm::mat4 m4Matrix = m_pCamera->GetViewMatrix();	
						glUniformMatrix4fv(uniform.second.ID, 1, GL_FALSE, &m4Matrix[0][0]);
						GL_GET_ERROR();
					}
					else if (uniform.first == "u_vp" || uniform.first == "u_pv")
					{
						const glm::mat4 m4Matrix = m_pCamera->GetProjectionViewMatrix();	
						glUniformMatrix4fv(uniform.second.ID, 1, GL_FALSE, &m4Matrix[0][0]);
						GL_GET_ERROR();
					}
				}

				GL_GET_ERROR();

				for (auto& mIBO : mShader.second)
				{
					/* IBO can be null */
					if (mIBO.first) mIBO.first->Bind();
					else IBO::Unbind();

					for (auto& mTexture : mIBO.second)
					{
						/* Texture can be null */
						if (mTexture.first) mTexture.first->Bind();
						else glBindTexture(GL_TEXTURE_2D, 0);

						for (auto& mRenderer : mTexture.second)
						{
							if (mRenderer->IsEnabled())
								mRenderer->Render(m_pCamera);
						}

						if (mTexture.first) mTexture.first->Unbind();
					}

					if (mIBO.first) IBO::Unbind();
				}

				mShader.first->DisableAttributes();
				ShaderProgram::Deactivate();
			}

			VBO::Unbind();
		}
	
		if (bIs2D)
		{
			glEnable(GL_CULL_FACE);
			glEnable(GL_DEPTH_TEST);
		}

		GL_GET_ERROR();
	}

	//Render lines
	glm::mat4 m4ProjView = m_pCamera->GetProjectionViewMatrix();
	if (m_pPhysicsDebugDrawer->GetLineCount() > 0)
	{
		m_pPhysicsDebugDrawer->Draw(m4ProjView);
	}

	// Render UI
	Application::GetInstance()->GetUIRenderer()->Render();

	glDisable(GL_BLEND);
}

void RenderSystem::OnComponentDestroyed(Component* a_pComponent)
{
	Renderer* pRenderer = static_cast<Renderer*>(a_pComponent);
	Texture* pTexture = pRenderer->GetTexture();
	IBO* pIBO = pRenderer->GetIBO();
	ShaderProgram* pShader = pRenderer->GetShader();
	VBO* pVBO = pRenderer->GetVBO();
	Component::Type eType = pRenderer->GetType();

	std::vector<Renderer*>& renderers = m_vRenderBatches[eType][pVBO][pShader][pIBO][pTexture];

	const std::vector<Renderer*>::iterator iter = std::find(
		renderers.begin(),
		renderers.end(),
		a_pComponent
	);

	if (iter != renderers.end())
		renderers.erase(iter);
}

void RenderSystem::OnComponentAdded(Component* a_pComponent)
{
	Renderer* pRenderer = static_cast<Renderer*>(a_pComponent);
	Texture* pTexture = pRenderer->GetTexture();
	IBO* pIBO = pRenderer->GetIBO();
	ShaderProgram* pShader = pRenderer->GetShader();
	VBO* pVBO = pRenderer->GetVBO();
	const Component::Type& eType = pRenderer->GetType();
	
	m_vRenderBatches[eType][pVBO][pShader][pIBO][pTexture].push_back(pRenderer);
}
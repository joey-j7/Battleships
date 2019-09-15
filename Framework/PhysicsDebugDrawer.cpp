#include "PhysicsDebugDrawer.h"
#include "ShaderProgram.h"
#include "Utils.h"
#include "Application.h"

#include "SceneManager.h"
#include "Scene.h"
#include "Camera.h"

PhysicsDebugDrawer* PhysicsDebugDrawer::m_pInstance = nullptr;

PhysicsDebugDrawer::PhysicsDebugDrawer(): m_iDebugMode(0)
{
	m_pShader = Application::GetInstance()->GetLoadInterface()->LoadShader(
		"Assets/Engine/Shaders/Debug Renderer/DebugRender3D.vsh",
		"Assets/Engine/Shaders/Debug Renderer/DebugRender3D.fsh"
	);

	m_uiLineCount = 0;

#ifdef DEBUG
	m_bDrawDebug = true;
#endif
}

void PhysicsDebugDrawer::Draw(glm::mat4& a_m4ViewProj)
{
	if (m_uiLineCount > 0)
	{
		m_pShader->Activate();

		for (const auto i : m_pShader->GetAttributes())
		{
			glEnableVertexAttribArray(i.second.ID);

			if (i.first == "a_position")
				glVertexAttribPointer(i.second.ID, i.second.Size, i.second.Type, i.second.Normalized, sizeof(DebugVertex), &(m_aVertexBuffer[0].Position[0]));
			else if (i.first == "a_debugcolor")
				glVertexAttribPointer(i.second.ID, i.second.Size, i.second.Type, i.second.Normalized, sizeof(DebugVertex), &(m_aVertexBuffer[0].Color[0]));
			else
				Debug::Log("Warning! Missing PhysicsDebugDrawer attribute: " + i.first);
		}

		for (auto p : m_pShader->GetUniforms())
		{
			if (p.first == "u_pv")
			{
				const glm::mat4 m4Matrix = Application::GetInstance()->GetSceneManager()->GetActiveScene()->GetCamera()->GetProjectionViewMatrix();	
				glUniformMatrix4fv(p.second.ID, 1, GL_FALSE, &m4Matrix[0][0]);
			}
			else
				Debug::Log("Warning! Missing PhysicsDebugDrawer uniform: " + p.first);
		}
		
		glDrawArrays(GL_LINES, 0, m_uiLineCount * 2);

		m_uiLineCount = 0;
		m_pShader->DisableAttributes();
		ShaderProgram::Deactivate();
	
		GL_GET_ERROR()
	}
}

void PhysicsDebugDrawer::drawLine(const btVector3& v3From, const btVector3& v3To, const btVector3& v3Color)
{
	if (m_uiLineCount < MAX_DEBUG_LINES)
	{
		m_aVertexBuffer[m_uiLineCount * 2].Position = glm::vec3(v3From.x(), v3From.y(), v3From.z());
		m_aVertexBuffer[m_uiLineCount * 2].Color = glm::vec3(v3Color.x(), v3Color.y(), v3Color.z());

		m_aVertexBuffer[m_uiLineCount * 2 + 1].Position = glm::vec3(v3To.x(), v3To.y(), v3To.z());
		m_aVertexBuffer[m_uiLineCount * 2 + 1].Color = glm::vec3(v3Color.x(), v3Color.y(), v3Color.z());

		++m_uiLineCount;
	}
}

void PhysicsDebugDrawer::reportErrorWarning(const char* pcWarningString)
{
	printf("%s", pcWarningString);
}

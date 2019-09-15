#include "Renderer.h"
#include "Mesh.h"
#include "Application.h"

Renderer::Renderer(GameObject* a_pGameObject, ShaderProgram* a_pShader) : 
	Component(a_pGameObject)
{
	m_pShader = a_pShader;
}

Renderer::~Renderer()
{
	if (m_pShader)
		Application::GetInstance()->GetLoadInterface()->DeleteShader(m_pShader->GetVertShader()->GetPath(), m_pShader->GetFragShader()->GetPath());

	if (m_pModel) 
		Application::GetInstance()->GetLoadInterface()->DeleteModel(m_pModel->GetPath(), m_pModel->GetType());

	else if (m_pTexture)
		Application::GetInstance()->GetLoadInterface()->DeleteTexture(m_pTexture->GetPath());
}

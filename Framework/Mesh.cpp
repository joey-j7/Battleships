#include "Mesh.h"

#include "Application.h"

#include "IBO.h"
#include "Texture.h"

Mesh::~Mesh()
{
	for (Texture* pTexture : GetTextures())
	{
		Application::GetInstance()->GetLoadInterface()->DeleteTexture(pTexture->GetPath());
	}

	delete m_pIBO;
}
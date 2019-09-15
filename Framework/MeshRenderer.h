#pragma once

#include "Model.h"
#include "Renderer.h"

class Shape;

class MeshRenderer : public Renderer
{
public:
	MeshRenderer(
		GameObject* a_pGameObject,
		const std::string& a_sModelPath,
		unsigned int a_uiMeshIndex = 0,
		Texture* a_pTexture = nullptr,
		const std::string& a_sVertPath = "Assets/Engine/Shaders/Default/Specular.vsh",
		const std::string& a_sFragPath = "Assets/Engine/Shaders/Default/Specular.fsh"
	);
  
	MeshRenderer(
		GameObject* a_pGameObject,
		Shape::Type a_eType,
		unsigned int a_uiSegments,
		Texture* a_pTexture,
		const std::string& a_sVertPath = "Assets/Engine/Shaders/Default/Specular.vsh",
		const std::string& a_sFragPath = "Assets/Engine/Shaders/Default/Specular.fsh"
	);

	virtual ~MeshRenderer();

	bool Render(Camera* a_pCamera) override;
	
	Mesh* GetMesh() const { return m_pModel->GetMeshes()[m_uiMeshIndex]; }

	VBO* GetVBO() const override;
	IBO* GetIBO() const override;

private:
	GameObject* m_pGameObject = nullptr;
	unsigned int m_uiMeshIndex = 0;

	Type m_eType = E_Render;

	Texture* m_pTextureLQ = nullptr;

	Shape* m_pShape = nullptr;

	VBO* m_pVBO;
	IBO* m_pIBO;

	bool m_bVisible = false;
};

#pragma once
#include "Renderer.h"

class Model;
class Camera;
class SkyboxRenderer : public Renderer
{
public:
	SkyboxRenderer(GameObject* a_pGameObject, Texture* a_pTexture);
	virtual ~SkyboxRenderer();

	bool Render(Camera* a_pCamera) override;

	VBO* GetVBO() const override;
	IBO* GetIBO() const override;

private:
	GameObject* m_pGameObject = nullptr;
	Camera* m_pCamera = nullptr;
};

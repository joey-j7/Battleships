#pragma once

#include "Renderer.h"

class Shape;

class SpriteRenderer : public Renderer
{
public:
	SpriteRenderer(GameObject* a_pGameObject, const std::string& a_sTexturePath, bool a_bScreenToWorld = true, bool a_bScaleResolution = true);
	virtual ~SpriteRenderer();

	bool Render(Camera* a_pCamera) override;

	VBO* GetVBO() const override;
	IBO* GetIBO() const override;

	void SetScale(float a_fScale)
	{
		m_fScale = a_fScale;
	}

private:
	glm::mat4 CreateTransformMatrix(Camera* a_pCamera);

	bool m_bScreenToWorld = true;
	bool m_bScaleResolution = true;

	GameObject* m_pGameObject = nullptr;
	Shape* m_pShape = nullptr;

	float m_fScale = 1.0f;
	bool m_bCenterOrigin = true;

	bool m_bVisible = true;
};

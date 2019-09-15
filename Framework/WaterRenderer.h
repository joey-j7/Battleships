#pragma once

#include "Renderer.h"

class Model;
class Shape;
class Camera;

class VBO;
class IBO;

class WaterRenderer : public Renderer
{
public:
	WaterRenderer(GameObject* a_pGameObject);
	virtual ~WaterRenderer();

	bool Render(Camera* a_pCamera) override;

	const glm::vec4& GetColor() const
	{
		return m_v4Color;
	}

	void SetColor(const glm::vec4& a_vColor)
	{
		m_v4Color = a_vColor;
	}

	VBO* GetVBO() const override;
	IBO* GetIBO() const override;

private:
	GameObject* m_pGameObject = nullptr;

	Camera* m_pCamera = nullptr;
	Shape* m_pQuad = nullptr;
	
	Texture* m_pDUDVTexture = nullptr;

	const glm::vec2 m_v2WaveSpeed = glm::vec2(0.03f, 10.0f);
	glm::vec2 m_v2WaveOffset = glm::vec2(0.0f, 0.0f);
	const float m_fWaveHeight = 0.2f;

	glm::vec4 m_v4Color = glm::vec4(0.0f, 77.0f, 75.0f, 131.0f) / 255.0f;
	float m_fHeightDistance = 0.0f;

	/* Texture Paths */
	static const std::string m_vCubeTexturesLQ[6];
};

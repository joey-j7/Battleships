#pragma once

#include "Renderer.h"
#include <string>

class VBO;

class ParticleRenderer : public Renderer
{
public:
	struct Data
	{	
		glm::vec3 Color;

		float Radius;
		float Velocity;
		float Size;
		float Degrees;

		unsigned int Count;
	
		float LifeTime;
		float TimeOffset;
	
		glm::vec3 Offset;
	};

	ParticleRenderer(GameObject* a_pGameObject, const Data& a_Data, const std::string& a_sFilePath = "");
	virtual ~ParticleRenderer();

	bool Render(Camera* a_pCamera) override;

	VBO* GetVBO() const override;
	IBO* GetIBO() const override;

private:
	VBO* m_pBuffer = nullptr;
	bool m_bVisible = false;

	float m_fTime = 0.f;
	float m_fInvLifeTime;

	Data m_Data;

	static const int m_iMaxParticles = 1000000;
};

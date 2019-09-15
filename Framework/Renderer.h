#pragma once
#include "Component.h"
#include <glm/glm.hpp>

class ShaderProgram;
class Mesh;
class Texture;
class Camera;

class VBO;
class IBO;
class Model;

class Renderer : public Component
{
public:
	Renderer(GameObject* a_pGameObject, ShaderProgram* a_pShader);
	virtual ~Renderer();

	ShaderProgram* GetShader() const { return m_pShader; }
	Texture* GetTexture() const { return m_pTexture; }
	Model* GetModel() const { return m_pModel; }

	virtual VBO* GetVBO() const = 0;
	virtual IBO* GetIBO() const = 0;

	virtual bool Render(Camera* a_pCamera) = 0;

	virtual float GetOpacity() const
	{
		return m_fOpacity;
	}
  
	virtual void SetOpacity(const float a_fOpacity)
	{
		m_fOpacity = glm::clamp(a_fOpacity, 0.0f, 1.0f);
	}
  
	virtual void SetColor(const glm::vec3& a_v3Color)
	{
		/* Convert to normalized / divide by 255 */
		m_v3Color = a_v3Color * 0.00392156863f;
	}
  
protected:
	Model* m_pModel = nullptr;

	ShaderProgram* m_pShader = nullptr;
	Texture* m_pTexture = nullptr;

	float m_fOpacity = 1.0f;
	glm::vec3 m_v3Color = glm::vec3(1.f, 1.f, 1.f);
};

#pragma once

#include <glm/detail/type_mat4x4.hpp>
#include <string>

class InputManager;

class ShaderProgram;
class Texture;
class Sprite;
class Model;
class Shape;

class UICursor
{
public:
	UICursor(const std::string& a_sFilePath = "Assets/Engine/Textures/UI/Cursor/Default.png");

	void Update();
	void Draw() const;

	void SetActive(bool a_bActive)
	{
		m_bActive = a_bActive;
	}

private:
	void Bind() const;
	void Unbind() const;

	glm::mat4 CreateTransformMatrix() const;

	InputManager* m_pInput;

	glm::vec3 m_v3Position;
	glm::vec3 m_v3Scale;

	ShaderProgram* m_pShader = nullptr;
	Texture* m_pTexture = nullptr;
	Shape* m_pShape = nullptr;

	glm::mat4 m_m4Ortho;

	bool m_bActive = false;
};

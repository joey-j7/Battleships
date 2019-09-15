#pragma once

#include <glm/glm.hpp>

class GameObject;
class SpriteRenderer;

class HPBar
{
public:
	HPBar(GameObject* a_pGameObject);

	void SetAsEnemy(bool a_bIsEnemy);

	void Update(unsigned int a_uiTargetHealth, unsigned int a_uiMaxHealth);

	void Show() { m_bShow = true; }
	void Hide() { m_bShow = false; }

private:
	void CalculateColor(float a_fPercentage) const;
	static glm::vec3 Colors[3];

	GameObject* m_pBackground;
	GameObject* m_pForeground;

	SpriteRenderer* m_pRendererBG;
	SpriteRenderer* m_pRendererFG;

	static glm::vec3 m_v3Size;
	static float m_fScale;

	static float m_fAnimationSpeed;
	static float m_fFadeSpeed;

	unsigned int m_uiCurrentHealth = 0;

	bool m_bShow = false;
	float m_fYOffset = 3.5f;
};

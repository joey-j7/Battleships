#include "HPBar.h"

#include "GameObject.h"
#include "Transform.h"
#include "SpriteRenderer.h"
#include "GameTime.h"

glm::vec3 HPBar::m_v3Size = glm::vec3(50.0f, 7.0f, 1.0f);
float HPBar::m_fScale = 2.0f;

float HPBar::m_fAnimationSpeed = 0.5f;
float HPBar::m_fFadeSpeed = 2.0f;

glm::vec3 HPBar::Colors[3]
{
	/* Red */
	glm::vec3(255, 140, 140),
	/* Orange */
	glm::vec3(255, 200, 180),
	/* Green */
	glm::vec3(180, 255, 200)
};

HPBar::HPBar(GameObject* a_pGameObject)
{
	/* Foreground Object */
	m_pForeground = GameObject::Create();
	m_pForeground->SetTag("Foreground");
	
	m_pRendererFG = new SpriteRenderer(m_pForeground, "Assets/Game/Textures/UI/HUD/HP Bar/Foreground.png");
	m_pRendererFG->SetScale(m_fScale);
	m_pRendererFG->SetOpacity(0.f);
	m_pRendererFG->SetColor(Colors[2]);
	m_pRendererFG->SetType(Component::E_Render2DAfter);
	m_pForeground->AddComponent(m_pRendererFG);

	Transform* pHPTransformFG = m_pForeground->GetTransform();
	pHPTransformFG->SetScale(
		glm::vec3(
			m_v3Size.x - 2.0f,
			m_v3Size.y - 2.0f,
			1.0f
		)
	);

	/* Background Object */
	m_pBackground = GameObject::Create();
	m_pBackground->SetTag("HUD");
  
	m_pRendererBG = new SpriteRenderer(m_pBackground, "Assets/Engine/Textures/white.png");
	m_pRendererBG->SetScale(m_fScale);
	m_pRendererBG->SetOpacity(0.f);
	
	m_pBackground->AddComponent(m_pRendererBG);
	
	Transform* pHPTransformBG = m_pBackground->GetTransform();
	pHPTransformBG->SetPosition(glm::vec3(0.0f, m_fYOffset, 0.0f));
	pHPTransformBG->SetScale(m_v3Size);
  
	/* Set Correct Parents */
	m_pBackground->SetParent(a_pGameObject);
	m_pForeground->SetParent(m_pBackground);
}

void HPBar::SetAsEnemy(bool a_bIsEnemy)
{
	m_pRendererBG->SetColor(glm::vec3((a_bIsEnemy) ? 75 : 0, 0, 0));
}

void HPBar::Update(unsigned int a_uiTargetHealth, unsigned int a_uiMaxHealth)
{
	/* Animate show / hide */
	float m_fOpacityBG = m_pRendererBG->GetOpacity();
	
	if (m_bShow && m_fOpacityBG != 1.0f)
	{
		m_fOpacityBG += GameTime::GetDeltaTime() * m_fFadeSpeed;

		m_fYOffset = 3.5f / m_pBackground->GetParent()->GetTransform()->GetScale().x;
		m_pBackground->GetTransform()->SetPosition(glm::vec3(0, m_fYOffset, 0));

		m_pRendererBG->SetOpacity(m_fOpacityBG);
		m_pRendererFG->SetOpacity(m_fOpacityBG);
	}
	else if (!m_bShow && m_fOpacityBG != 0.0f)
	{
		m_fOpacityBG -= GameTime::GetDeltaTime() * m_fFadeSpeed;

		m_fYOffset += 0.2f / m_pBackground->GetParent()->GetTransform()->GetScale().x;
		m_pBackground->GetTransform()->SetPosition(glm::vec3(0, m_fYOffset, 0));
	
		m_pRendererBG->SetOpacity(m_fOpacityBG);
		m_pRendererFG->SetOpacity(m_fOpacityBG);
	}

	/* Early out if HP is up to date */
	if (a_uiTargetHealth == m_uiCurrentHealth)
		return;

	/* Animate health bar animation */
	const int fStep = std::ceil(m_fAnimationSpeed * a_uiMaxHealth * GameTime::GetDeltaTime());
	
	if (a_uiTargetHealth > m_uiCurrentHealth)
	{
		m_uiCurrentHealth += fStep;
		m_uiCurrentHealth = glm::min(m_uiCurrentHealth, a_uiMaxHealth);
	}
	else if (a_uiTargetHealth < m_uiCurrentHealth)
	{
		if (fStep > m_uiCurrentHealth) m_uiCurrentHealth = 0;
		else m_uiCurrentHealth -= fStep;

		if (m_uiCurrentHealth < a_uiTargetHealth)
			m_uiCurrentHealth = a_uiTargetHealth;
	}

	/* Apply health to bar */
	Transform* pTransform = m_pForeground->GetTransform();
	const glm::vec3& v3Scale = pTransform->GetScale();
	const float fPercentage = (float)m_uiCurrentHealth / (float)a_uiMaxHealth;

	pTransform->SetScale(
		glm::vec3(
			fPercentage * (m_v3Size.x - 4.0f),
			v3Scale.y,
			v3Scale.z
		)
	);
  
	CalculateColor(fPercentage);
}

void HPBar::CalculateColor(float a_fPercentage) const
{
	/* Lerp between 3 colors */
	a_fPercentage *= 3;

	const int iColorIndex = a_fPercentage;
	glm::vec3 v3Color = Colors[iColorIndex];

	if (iColorIndex > 0)
	{
		a_fPercentage = 1.0f - std::fmod(a_fPercentage, 1.0f);
		v3Color = glm::lerp(v3Color, Colors[iColorIndex - 1], a_fPercentage);
	}

	m_pRendererFG->SetColor(v3Color);
}

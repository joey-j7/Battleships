#pragma once

#include "UIElement.h"

namespace UIElements
{
	class GameObjectProperties : public UIElement
	{
	public:
		GameObjectProperties() : UIElement("Game Object Properties", false, E_WINDOW, UIRenderer::E_DEBUG) {};

		void Update() override;
		void Render() override;

		virtual void SetActive(bool a_bActive) override;

	private:
		void ShowObjectItem(GameObject* a_pObject, int a_iIndex);

		glm::vec3 m_v3SnapTranslation;
		glm::vec3 m_v3SnapRotation;
		glm::vec3 m_v3SnapScale;
	};
}
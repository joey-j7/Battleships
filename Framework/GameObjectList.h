#pragma once

#include "UIElement.h"

namespace UIElements
{
	class GameObjectList : public UIElement
	{
	public:
		GameObjectList() : UIElement("Game Object List", false, E_WINDOW, UIRenderer::E_DEBUG, Input::E_F2) {};
		void Render() override;

	private:
		void ShowObjectItem(GameObject* a_pObject, int a_iIndex);
	};
}
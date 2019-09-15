#pragma once

#include "UIElement.h"

namespace UIElements
{
	class WaterWindow : public UIElement
	{
	public:
		WaterWindow() : UIElement("Water Window", false, E_WINDOW, UIRenderer::E_DEBUG, Input::E_F7) {};
		void Render() override;
	};
}
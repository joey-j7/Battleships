#pragma once

#include "UIElement.h"

namespace UIElements
{
	class NetworkWindow : public UIElement
	{
	public:
		NetworkWindow() : UIElement("Network Window", false, E_WINDOW, UIRenderer::E_DEBUG, Input::E_F5) {};
		void Render() override;
	};
}
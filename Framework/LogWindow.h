#pragma once

#include "UIElement.h"

namespace UIElements
{
	class LogWindow : public UIElement
	{
	public:
		LogWindow() : UIElement("Log Window", false, E_WINDOW, UIRenderer::E_DEBUG, Input::E_F4) {};
		void Render() override;
	};
}
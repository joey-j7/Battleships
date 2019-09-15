#pragma once

#include "UIElement.h"

namespace UIElements
{
	class GameProperties : public UIElement
	{
	public:
		GameProperties() : UIElement("Game Settings", true, E_FIXED_WINDOW) {};
		void Render() override;
	};
}
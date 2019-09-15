#pragma once

#include "UIElement.h"

namespace UIElements
{
	class ExitDialog : public UIElement
	{
	public:
		ExitDialog() : UIElement("Exit Game", false, E_DIALOG, UIRenderer::E_EXIT_DIALOG) {};
		
		void Render() override;
		void Update() override;

		static void Activate()
		{
		  m_bToggle = true;
		}

	private:
		ImGuiTextBuffer m_LogTextBuffer;
		static bool m_bToggle;
	};
}
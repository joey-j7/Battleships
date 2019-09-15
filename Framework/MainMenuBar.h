#pragma once

#include "UIElement.h"

class Scene;

namespace UIElements
{
	class MainMenuBar : public UIElement
	{
	public:
		MainMenuBar() : UIElement("Main Menu Bar", false, E_MAINMENUBAR, UIRenderer::E_DEBUG, Input::E_F1) {};
		void Render() override;

	private:
		void ShowFileMenu();
		void ShowViewMenu();

		bool m_bGotoTitle = false;
	};
}
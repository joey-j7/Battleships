#include "MainMenuBar.h"
#include "GameTime.h"

#include "InputManager.h"
#include "GameObject.h"

void MainMenuBar::Render()
{
	ShowFileMenu();
	ShowViewMenu();

	ImGui::SameLine(ImGui::GetWindowWidth() - 90 - ImGui::GetStyle().WindowPadding.x);
	ImGui::TextColored(ImVec4{ 1.0f, 0.7f, 0.7f, 1.0f }, "FPS: %.1f", GameTime::GetFPS());

	if (m_bGotoTitle) {
		//Create load screen object
		Application::GetInstance()->ReturnToTitle();

		m_bGotoTitle = false;
		SetActive(false);
	}
}

void MainMenuBar::ShowFileMenu()
{
	if (ImGui::BeginMenu("File"))
	{
		m_bGotoTitle = ImGui::MenuItem("Return to Title");

		if (ImGui::MenuItem("Exit Game", "ESC"))
		{
			m_pUIRenderer->Exit();
		}

		ImGui::EndMenu();
	}
}

void MainMenuBar::ShowViewMenu()
{
	if (ImGui::BeginMenu("View"))
	{
		bool bActive = m_bActive;

		if (ImGui::MenuItem("Debug UI", "F1", &m_bActive))
			SetActive(bActive);
	
		for (UIElement* pElement : m_pChildren)
		{
			bActive = pElement->IsActive();
			if (ImGui::MenuItem(
					pElement->GetName().c_str(),
					m_pInput->KeycodeToString(pElement->GetToggleKey()).c_str(), 
					&bActive
				)
			)
				pElement->SetActive(bActive);
		}

		ImGui::EndMenu();
	}
}

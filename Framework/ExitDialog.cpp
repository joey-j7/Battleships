#include "ExitDialog.h"
#include "Application.h"

bool ExitDialog::m_bToggle = false;

void ExitDialog::Render()
{
	ImGui::SetWindowFontScale(1.2);

	ImGui::Text("Do you really want to quit the game?");
	ImGui::Spacing();

	if (ImGui::Button("Yes", ImVec2(60, 30)))
	{
		m_pApp->Exit();
	}

	ImGui::SameLine();

	if (ImGui::Button("No", ImVec2(60, 30)))
	{
		SetActive(false);
	}
}

void ExitDialog::Update()
{
	if ((m_bToggle && !IsActive()) || m_pInput->KeyPressed(Input::E_ESC))
	{
		SetActive(!IsActive());
		m_bToggle = false;
	}
}

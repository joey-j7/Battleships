#include "MainMenuWindow.h"
#include "Application.h"
#include "imgui_internal.h"

MainMenuWindow::MainMenuWindow() :
	UIElement("Main Menu", true, E_FIXED_WINDOW)
{
}

void MainMenuWindow::Render()
{
	ImGui::SetWindowFontScale(1.2);

	ImGui::Indent(125.0f);
	if (ImGui::Button("Host Game", ImVec2(200, 50)))
		OnHostLobbyPressed();

	if (ImGui::Button("Join Game", ImVec2(200, 50)))
		OnJoinGamePressed();

	ImGui::NewLine();

#ifdef DEBUG
	if (ImGui::Button("Debug Scene", ImVec2(200, 50)))
		OnDebugPressed();
#endif

	if (ImGui::Button("Settings", ImVec2(200, 50)))
		OnSettingsPressed();

	ImGui::Unindent(125.0f);

	ImGui::NewLine();

	if (ImGui::Button("Credits", ImVec2(200, 50)))
		OnCreditsPressed();

	ImGui::SameLine(250);

	if (ImGui::Button("Quit", ImVec2(200, 50)))
		m_pApp->Exit();
}

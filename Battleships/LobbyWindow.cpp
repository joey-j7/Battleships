#include "LobbyWindow.h"
#include "Application.h"
#include "NetworkClient.h"
#include "imgui_internal.h"

LobbyWindow::LobbyWindow() :
	UIElement("Lobby", true, E_FIXED_WINDOW)
{
	m_pClient = Application::GetInstance()->GetClient();
	m_bEnableConnectionInterface = false;
}

void LobbyWindow::Render()
{
	const std::vector<NetworkUser>& connections = m_pClient->GetConnections();
	ImGui::SetWindowFontScale(1.2);

	ImGui::Indent(450.0f);
	ImGui::Unindent(450.0f);

	if (m_pClient->IsHost()) 
	{
		if (connections.size() == 0)
			ImGui::Text("Hosting now. Searching for another player...");
		else
			ImGui::Text("Another player is connected, start the game.");

		ImGui::Text(std::string("Local Host IP: " + m_pClient->GetLocalIP()).c_str());
	}
	else
	{
		if (connections.size() == 0) {
		ImGui::Text("Please enter the IP address to connect to:");
		}
		else
		ImGui::Text("Connected to host. Waiting for host to start...");
	}

	for (const NetworkUser& user : connections)
	{
		ImGui::Text(user.Name.c_str());
	}

	static char buffer[32] = "";
	if (m_bEnableConnectionInterface)
	{
		ImGui::NewLine();
		ImGui::InputTextEx("", buffer, IM_ARRAYSIZE(buffer), ImVec2(450, 40), ImGuiInputTextFlags_CharsDecimal);
	}

	if (connections.size() == 0 && !m_pClient->IsHost())
	{
		const std::string& sConnectionMessage = m_pClient->GetConnectionMessage();

		if (!sConnectionMessage.empty()) {
		ImGui::Text(sConnectionMessage.c_str());
		}
	}

	ImGui::NewLine();

	const int windowSizeX = ImGui::GetCurrentWindow()->Size.x - ImGui::GetCurrentWindow()->WindowPadding.x * 2;
	const int numBtn = connections.size() > 0 && m_pClient->IsHost() ? 2 : 1;
	ImGui::Indent(windowSizeX / 2.f - 100 * numBtn);

	if (m_bEnableConnectionInterface)
	{
		if (ImGui::Button("Connect", ImVec2(200, 50)))
		{
			OnConnectPressed(buffer);
		}
	}

	if (ImGui::Button("Disconnect", ImVec2(200, 50)))
	{
		OnDisconnectPressed();
	}

	if (connections.size() > 0 && m_pClient->IsHost())
	{
		ImGui::SameLine();
		if (ImGui::Button("Start Game", ImVec2(200, 50)))
		{
			OnStartGamePressed();
		}
	}

	ImGui::Unindent(windowSizeX / 2.f - 100);
}
	
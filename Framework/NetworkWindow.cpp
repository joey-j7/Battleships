#include "NetworkWindow.h"
#include "Application.h"

void NetworkWindow::Render()
{
	static char buffer[32] = "";
	ImGui::InputText("IP Address", buffer, IM_ARRAYSIZE(buffer));

	if (ImGui::Button("Connect", ImVec2(200, 50)))
	{
		m_pApp->GetClient()->Connect(buffer);
	}
}

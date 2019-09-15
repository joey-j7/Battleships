#include "LogWindow.h"
#include "Debug.h"

using namespace UIElements;

void LogWindow::Render()
{
  #ifdef DEBUG
	ImGui::BeginChild("scrolling");
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 1));
	ImGui::PushTextWrapPos();

	ImGui::TextUnformatted(Debug::LogTextBuffer.begin());

	ImGui::PopTextWrapPos();
	ImGui::SetScrollHere(1.0f);
	ImGui::PopStyleVar();
	ImGui::EndChild();
  #endif
}
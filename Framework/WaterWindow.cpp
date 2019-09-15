#include "WaterWindow.h"

#include "WaterRenderer.h"
#include "Application.h"

void WaterWindow::Render()
{
	GameObject* pObject = GameObject::FindGameObjectWithTag("Water Surface");
	if (!pObject) return;

	WaterRenderer* pRenderer = pObject->GetComponent<WaterRenderer>();
	glm::vec4 v4Color = pRenderer->GetColor();

	if (ImGui::ColorPicker4("Water Color", &v4Color[0], ImGuiColorEditFlags_AlphaBar))
		pRenderer->SetColor(v4Color);
}

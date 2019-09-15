#include "GameProperties.h"
#include "Application.h"
#include "RenderSystem.h"

#include "imgui_internal.h"
#include "PhysicsDebugDrawer.h"

void GameProperties::Render()
{
	ImGui::SetWindowFontScale(1.2);

	GLContext* pGLContext = m_pApp->GetGLContext();
	bool bHasVSync = pGLContext->HasVSync();

	if (ImGui::Checkbox("Vertical Sync", &bHasVSync))
	{
		pGLContext->SetVSync(bHasVSync);
	}

#ifdef WINDOWS
	bool bFullScreen = pGLContext->IsFullScreen();

	if (ImGui::Checkbox("Full Screen", &bFullScreen))
	{
		pGLContext->SetFullScreen(bFullScreen);
	}

	bool bIsHighQuality = m_pApp->IsHighQuality();

	if (ImGui::Checkbox("High Quality Graphics", &bIsHighQuality))
	{
		m_pApp->SetHighQuality(bIsHighQuality);
	}
#endif

#ifdef DEBUG
	bool bDrawDebug = PhysicsDebugDrawer::GetInstance()->DoesDrawDebug();

	if (ImGui::Checkbox("Draw Debug Lines", &bDrawDebug))
	{
		PhysicsDebugDrawer::GetInstance()->SetDebugDraw(bDrawDebug);
	}
#endif

	ImGui::NewLine();

	const int windowSizeX = ImGui::GetCurrentWindow()->Size.x - ImGui::GetCurrentWindow()->WindowPadding.x * 2;
	ImGui::Indent(windowSizeX * 0.5f - 100);

	if (ImGui::Button("Save Settings", ImVec2(200, 50)))
	{
		EnablePrevious();
		SetActive(false);
	}
}

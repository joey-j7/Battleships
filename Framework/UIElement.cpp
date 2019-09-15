#include "UIElement.h"
#include "Application.h"

#include <algorithm>

using namespace UIElements;

UIElement::UIElement(const std::string& a_sName, bool a_bHideClose, Type a_eType, UIRenderer::Layer a_eLayer, Input::KeyCode a_eToggleKey)
{
	m_sName = a_sName;
	m_eType = a_eType;
	m_eLayer = a_eLayer;
	m_eToggleKey = a_eToggleKey;

	m_bActive = true;
	m_bHideClose = a_bHideClose;

	m_pApp = Application::GetInstance();
	m_pInput = m_pApp->GetInput();
	m_pUIRenderer = UIRenderer::GetInstance();

	m_pUIRenderer->AddElement(a_eLayer, this);
}

void UIElement::Destroy()
{
	m_pUIRenderer->RemoveElement(this);
}

void UIElement::Update()
{
	if (m_eToggleKey != Input::E_NONE && m_pInput->KeyPressed(m_eToggleKey))
	{
		SetActive(!IsActive());
	}
}

bool UIElement::Begin()
{
	bool bReturn;
	bool bActive = m_bActive;

	if (m_eType == E_MAINMENUBAR) {
		bReturn = ImGui::BeginMainMenuBar();
	}
	else if (m_eType == E_DIALOG)
	{
		ImGuiIO& io = ImGui::GetIO();
		ImGui::SetNextWindowPos(ImVec2(
			io.DisplaySize.x * 0.5f,
			io.DisplaySize.y * 0.5f
		), 0, ImVec2(0.5f, 0.5f));

		ImGui::SetWindowSize(ImVec2(0, 0));
		bReturn = ImGui::Begin(m_sName.c_str(), m_bHideClose ? NULL : &bActive, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
	}
	else if (m_eType == E_FIXED_WINDOW)
	{
		ImGuiIO& io = ImGui::GetIO();
		ImGui::SetNextWindowPos(ImVec2(
			io.DisplaySize.x * 0.5f,
			io.DisplaySize.y * 0.5f
		), 0, ImVec2(0.5f, 0.5f));

		ImGui::SetWindowSize(ImVec2(0, 0));
		bReturn = ImGui::Begin(m_sName.c_str(), m_bHideClose ? NULL : &bActive, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);
	}
	else
	{
		ImGui::SetWindowSize(ImVec2(0, 0));
		bReturn = ImGui::Begin(m_sName.c_str(), m_bHideClose ? NULL : &bActive);
	}

	SetActive(bActive);
	return bReturn;
}

void UIElement::End()
{  
	if (m_eType == E_MAINMENUBAR)
		ImGui::EndMainMenuBar();
	else
		ImGui::End();
}

void UIElement::SetActive(bool a_bActive)
{
	if (m_bActive == a_bActive)
		return;

	m_bActive = a_bActive;

	if (!m_bActive)
	{
		for (UIElement* pWindow : m_pChildren)
		{
			pWindow->SetActive(m_bActive);
		}
	}
	else if (m_pParent)
	{
		m_pParent->SetActive(m_bActive);
	}
}

void UIElement::AddChild(UIElement* a_pElement)
{
	/* Don't support elements that are aleady parented */
	if (a_pElement->GetParent())
		return;

	const auto itIter = std::find(m_pChildren.begin(), m_pChildren.end(), a_pElement);

	if (itIter == m_pChildren.end())
	{
		m_pChildren.push_back(a_pElement);
		a_pElement->SetParent(this);
	}
}

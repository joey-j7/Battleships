#include "UIRenderer.h"

#include "imgui.h"
#include "ImGuizmo.h"

#include "Application.h"
#include "GameTime.h"
#include "Camera.h"

#include "SceneManager.h"

#include <vector>
#include "ShaderProgram.h"

#include "Utils.h"
#include "GLWindow.h"

#include "VBO.h"
#include "IBO.h"

#include "UIElement.h"
#include "LogWindow.h"
#include "ExitDialog.h"
#include "MainMenuBar.h"
#include "GameObjectList.h"
#include "GameObjectProperties.h"
#include "NetworkWindow.h"
#include "WaterWindow.h"

#ifndef RASPBERRY
#include "imgui_impl_glfw_gl2.h"
#endif

UIRenderer* UIRenderer::m_pInstance = nullptr;

void ImGuiRenderer(ImDrawData* draw_data)
{
	const UIRenderer* pRenderer = UIRenderer::GetInstance();
	const ShaderProgram* pShader = pRenderer->GetShader();

	VBO* pVBO = pRenderer->GetVBO();
	IBO* pIBO = pRenderer->GetIBO();

	// Backup GL state
	GLint last_program, last_texture, last_array_buffer, last_element_array_buffer, last_viewport[4];
	glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
	glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_element_array_buffer);
	glGetIntegerv(GL_VIEWPORT, last_viewport);

	// Handle cases of screen coordinates != from framebuffer coordinates (e.g. retina displays)
	ImGuiIO& io = ImGui::GetIO();
	float fb_width = io.DisplaySize.x * io.DisplayFramebufferScale.x;
	float fb_height = io.DisplaySize.y * io.DisplayFramebufferScale.y;
	
    if (fb_width == 0 || fb_height == 0)
        return;

	// Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glActiveTexture(GL_TEXTURE0);

	glViewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);

	// Setup orthographic projection matrix
	const float ortho_projection[4][4] =
    {
        { 2.0f/io.DisplaySize.x, 0.0f,                   0.0f, 0.0f },
        { 0.0f,                  2.0f/-io.DisplaySize.y, 0.0f, 0.0f },
        { 0.0f,                  0.0f,                  -1.0f, 0.0f },
        {-1.0f,                  1.0f,                   0.0f, 1.0f },
    };

	pShader->Activate();

	for (auto& pUniform : pShader->GetUniforms())
	{
		if (pUniform.first == "u_texture")
			glUniform1i(pUniform.second.ID, 0);
		else if (pUniform.first == "u_projection")
			glUniformMatrix4fv(pUniform.second.ID, 1, GL_FALSE, &ortho_projection[0][0]);
		else
			Debug::Log("Warning! Missing UIRenderer uniform: " + pUniform.first);
	}

	// Render command lists
	pVBO->Bind();
	pIBO->Bind();

	const size_t szSize = VertexData::GetSize(VertexData::E_IMGUI);
	const std::vector<int> vOffsets = VertexData::GetOffsets(VertexData::E_IMGUI);

	for (auto& pAttribute : pShader->GetAttributes())
	{
		glEnableVertexAttribArray(pAttribute.second.ID);

		if (pAttribute.first == "a_position")
			glVertexAttribPointer(pAttribute.second.ID, 2, GL_FLOAT, GL_FALSE, szSize, (GLvoid*)vOffsets[0]);
		else if (pAttribute.first == "a_uv")
			glVertexAttribPointer(pAttribute.second.ID, 2, GL_FLOAT, GL_FALSE, szSize, (GLvoid*)vOffsets[1]);
		else if (pAttribute.first == "a_color")
			glVertexAttribPointer(pAttribute.second.ID, 4, GL_UNSIGNED_BYTE, GL_TRUE, szSize, (GLvoid*)vOffsets[2]);
		else
			Debug::Log("Warning! Missing UIRenderer attribute: " + pAttribute.first);
	}

	for (int n = 0; n < draw_data->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = draw_data->CmdLists[n];
		const ImDrawIdx* idx_buffer_offset = 0;

		glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmd_list->VtxBuffer.size() * sizeof(ImDrawVert), (GLvoid*)&cmd_list->VtxBuffer.front(), GL_STREAM_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd_list->IdxBuffer.size() * sizeof(ImDrawIdx), (GLvoid*)&cmd_list->IdxBuffer.front(), GL_STREAM_DRAW);
        
		for (const ImDrawCmd* pcmd = cmd_list->CmdBuffer.begin(); pcmd != cmd_list->CmdBuffer.end(); pcmd++)
		{
			if (pcmd->UserCallback)
			{
				pcmd->UserCallback(cmd_list, pcmd);
			}
			else
			{
				glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
				glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, GL_UNSIGNED_SHORT, idx_buffer_offset);
			}
			idx_buffer_offset += pcmd->ElemCount;
		}
	}
  
	GL_GET_ERROR();

	// Restore modified state
	pShader->DisableAttributes();
	ShaderProgram::Deactivate();

	glUseProgram(last_program);
	glBindTexture(GL_TEXTURE_2D, last_texture);
	glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, last_element_array_buffer);
	glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	GL_GET_ERROR();
}

UIRenderer::UIRenderer()
{
	m_pInstance = this;

	m_pApp = Application::GetInstance();
	m_pGLContext = m_pApp->GetGLContext();
	m_pInput = m_pApp->GetInput();

	/* Add default elements */
	ExitDialog* pExitDialog = new ExitDialog();
	pExitDialog->SetActive(false);

#ifdef DEBUG
	MainMenuBar* pMainMenuBar = new MainMenuBar();
	GameObjectList* pGameObjectList = new GameObjectList();
	
	GameObjectProperties* pGameObjectProperties = new GameObjectProperties();
	pGameObjectList->AddChild(pGameObjectProperties);
	
	LogWindow* pLogWindow = new LogWindow();
	NetworkWindow* pNetworkWindow = new NetworkWindow();
	WaterWindow* pWaterWindow = new WaterWindow();

	pMainMenuBar->AddChild(pGameObjectList);
	pMainMenuBar->AddChild(pLogWindow);
	pMainMenuBar->AddChild(pNetworkWindow);
	pMainMenuBar->AddChild(pWaterWindow);

	pMainMenuBar->SetActive(false);
#endif

	/* Create Context */
	ImGui::CreateContext();
}

UIRenderer::~UIRenderer()
{
	ClearElements(true);

	ImGui::DestroyContext();

	delete m_pFontTexture;

	delete m_pVBO;
	delete m_pIBO;
}

void UIRenderer::Init()
{ 
	/* Create Shader */
	m_pShader = m_pApp->GetLoadInterface()->LoadShader(
		"Assets/Engine/Shaders/ImGui/imgui.vsh",
		"Assets/Engine/Shaders/ImGui/imgui.fsh"
	);

	/* Create Buffers */
	m_pVBO = new VBO(VertexData::E_IMGUI);
	m_pIBO = new IBO();

	/* Create Font Texture */
	InitFont();
	InitStyle();

	MapKeys();
}

void UIRenderer::InitFrame()
{
	ImGuiIO& rImGuiIO = ImGui::GetIO();
	const glm::uvec2& uv2Size = m_pGLContext->GetGLWindow()->GetSize();

	float fScale = glm::min(GLWindow::GetDPIScale().x, GLWindow::GetDPIScale().y) * GLWindow::GetScale();
	if (fScale == 0.f) fScale = 1.f;

	rImGuiIO.DisplaySize = ImVec2((float)uv2Size.x / fScale, (float)uv2Size.y / fScale);

	rImGuiIO.DeltaTime = GameTime::GetDeltaTime();
	
	/* DPI / Resolution Awareness */
	rImGuiIO.DisplayFramebufferScale = ImVec2(fScale, fScale);

	ImGui::NewFrame();
	ImGuizmo::BeginFrame();
}

void UIRenderer::Render()
{
	//Delete UIElements that have been marked for removal
	if (m_vDeleteQueue.size() > 0)
	{
		for (UIElement* pElement : m_vDeleteQueue)
			DeleteElement(pElement);

		m_vDeleteQueue.clear();
	}

	bool bUpdated = false;

	/* Update and draw all element layers */
	for (auto itIter : m_mLayers)
	{
		#ifndef DEBUG
		if (itIter.first == E_DEBUG)
			continue;
		#endif

		for (UIElement* pElement : itIter.second)
		{
			pElement->Update();

			if (pElement->IsActive())
			{
				/* Update once */
				if (!bUpdated)
				{
					ImGuiIO& rImGuiIO = ImGui::GetIO();
		
					SyncMouse(rImGuiIO);
					SyncKeys(rImGuiIO);

					bUpdated = true;
				}
				
				if (pElement->Begin())
				{
					pElement->Render();
				}
				pElement->End();
			}
		}
	}

#ifdef ANDROID
  ImGuiIO& rImGuiIO = ImGui::GetIO();

  if (rImGuiIO.WantTextInput && !m_bKeyboardShown) {
    Application::GetInstance()->GetInput()->ShowKeyboard(true);
    m_bKeyboardShown = true;
  }
  else if (!rImGuiIO.WantTextInput && m_bKeyboardShown) {
    Application::GetInstance()->GetInput()->ShowKeyboard(false);
    m_bKeyboardShown = false;
  }
#endif
  
	ImGui::Render();
}

void UIRenderer::Exit()
{
	UIElement* pExitDialog = m_mLayers[E_EXIT_DIALOG].front();
	pExitDialog->SetActive(!pExitDialog->IsActive());
}

void UIRenderer::MapKeys()
{
	ImGuiIO& rImGuiIO = ImGui::GetIO();

	rImGuiIO.KeyMap[ImGuiKey_Tab] = Input::E_TAB;
	rImGuiIO.KeyMap[ImGuiKey_LeftArrow] = Input::E_LEFT;
	rImGuiIO.KeyMap[ImGuiKey_RightArrow] = Input::E_RIGHT;
	rImGuiIO.KeyMap[ImGuiKey_UpArrow] = Input::E_UP;
	rImGuiIO.KeyMap[ImGuiKey_DownArrow] = Input::E_DOWN;
	rImGuiIO.KeyMap[ImGuiKey_PageUp] = Input::E_PAGE_UP;
	rImGuiIO.KeyMap[ImGuiKey_PageDown] = Input::E_PAGE_DOWN;
	rImGuiIO.KeyMap[ImGuiKey_Home] = Input::E_HOME;
	rImGuiIO.KeyMap[ImGuiKey_End] = Input::E_END;
	rImGuiIO.KeyMap[ImGuiKey_Insert] = Input::E_INSERT;
	rImGuiIO.KeyMap[ImGuiKey_Delete] = Input::E_DELETE;
	rImGuiIO.KeyMap[ImGuiKey_Backspace] = Input::E_BACKSPACE;
	rImGuiIO.KeyMap[ImGuiKey_Space] = Input::E_SPACE;
	rImGuiIO.KeyMap[ImGuiKey_Enter] = Input::E_ENTER;
	rImGuiIO.KeyMap[ImGuiKey_Escape] = Input::E_ESC;
	rImGuiIO.KeyMap[ImGuiKey_A] = Input::E_A;
	rImGuiIO.KeyMap[ImGuiKey_C] = Input::E_C;
	rImGuiIO.KeyMap[ImGuiKey_V] = Input::E_V;
	rImGuiIO.KeyMap[ImGuiKey_X] = Input::E_X;
	rImGuiIO.KeyMap[ImGuiKey_Y] = Input::E_Y;
	rImGuiIO.KeyMap[ImGuiKey_Z] = Input::E_Z;

	rImGuiIO.KeyMap[ImGuiDir_Left] = Input::E_LEFT;
	rImGuiIO.KeyMap[ImGuiDir_Right] = Input::E_RIGHT;
	rImGuiIO.KeyMap[ImGuiDir_Up] = Input::E_UP;
	rImGuiIO.KeyMap[ImGuiDir_Down] = Input::E_DOWN;
}

void UIRenderer::RemoveElement(UIElement* a_pElement)
{
	m_vDeleteQueue.push_back(a_pElement);
}

void UIRenderer::AddElement(Layer a_Layer, UIElement* a_pWindow)
{
	m_mLayers[a_Layer].push_back(a_pWindow);
}

void UIRenderer::ClearElements(bool a_bClearDebug)
{
	for (auto itIter : m_mLayers)
	{
		if (a_bClearDebug || itIter.first != E_DEBUG)
		{
			for (UIElement* pElement : itIter.second)
			{
				delete pElement;
			}

			itIter.second.clear();
		}
	}
}

void UIRenderer::DeleteElement(UIElement* a_pElement)
{
	for (auto& itIter : m_mLayers)
	{
		std::vector<UIElement*>::iterator iter = itIter.second.begin();
		for (; iter != itIter.second.end(); ++iter)
		{
			if (a_pElement == *iter)
			{
				itIter.second.erase(iter);
				delete a_pElement;
				return;
			}
		}
	}
}

void UIRenderer::InitFont()
{
	GLint uiTextureID;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &uiTextureID);

	unsigned char* pucPixels;
	int iWidth, iHeight;

	ImGuiIO& io { ImGui::GetIO() };

	io.Fonts->AddFontFromFileTTF(GetPlatformPath("Assets/Engine/Fonts/OpenSans-Regular.ttf").c_str(), 48.0f);
	io.Fonts->GetTexDataAsRGBA32(&pucPixels, &iWidth, &iHeight);
	io.IniFilename = NULL;
	
	// Upload texture to graphics system, use path as relevant ID
	m_pFontTexture = new Texture();
	m_pFontTexture->Load(GetPlatformPath("Assets/Engine/Fonts/OpenSans-Regular.ttf").c_str(), pucPixels, iWidth, iHeight);

	// Get display size
	io.RenderDrawListsFn = ImGuiRenderer;

	io.Fonts->TexID = (void *)(intptr_t)m_pFontTexture->GetID();

	// Restore state
	glBindTexture(GL_TEXTURE_2D, uiTextureID);

	// Cleanup (don't clear the input data if you want to append new fonts later)
	io.Fonts->ClearInputData();
	io.Fonts->ClearTexData();
}

void UIRenderer::InitStyle()
{
	ImGui::GetIO().FontGlobalScale = 0.5f;
	ImGuiStyle * style = &ImGui::GetStyle();
 
	style->WindowPadding = ImVec2(15, 15);
	style->WindowRounding = 0.0f;
	style->WindowBorderSize = 0.0f;
	style->WindowTitleAlign = ImVec2(0.5f, 0.5f);
	style->FramePadding = ImVec2(5, 5);
	style->FrameRounding = 4.0f;
	style->ItemSpacing = ImVec2(12, 8);
	style->ItemInnerSpacing = ImVec2(8, 6);
	style->IndentSpacing = 25.0f;
	style->ScrollbarSize = 15.0f;
	style->ScrollbarRounding = 9.0f;
	style->GrabMinSize = 5.0f;
	style->GrabRounding = 3.0f;
	style->WindowMinSize = ImVec2(400, 100);
 
	style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
	style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 0.45f);
	style->Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_Border] = ImVec4(0.07f, 0.07f, 0.09f, 0.80f);
	style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
	style->Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.30f, 0.30f, 0.30f, 0.75f);
	style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 0.85f);
	style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_Column] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ColumnHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_ColumnActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
	style->Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);
}

void UIRenderer::SyncMouse(ImGuiIO& a_rImGuiIO)
{
	const glm::vec2& v2MousePosition = m_pApp->GetInput()->GetMousePosition();
	a_rImGuiIO.MousePos = ImVec2(v2MousePosition.x, v2MousePosition.y);
	
	a_rImGuiIO.MousePos.x *= GLWindow::GetSizeInv().x;
	a_rImGuiIO.MousePos.y *= GLWindow::GetSizeInv().y;
	
	a_rImGuiIO.MousePos.x *= a_rImGuiIO.DisplaySize.x;
	a_rImGuiIO.MousePos.y *= a_rImGuiIO.DisplaySize.y;

	a_rImGuiIO.MouseDown[0] = m_pApp->GetInput()->MouseDown(Input::E_MOUSE_LEFT);
	a_rImGuiIO.MouseDown[1] = m_pApp->GetInput()->MouseDown(Input::E_MOUSE_RIGHT);
	a_rImGuiIO.MouseDown[2] = m_pApp->GetInput()->MouseDown(Input::E_MOUSE_MIDDLE);
	
	//rImGuiIO.MouseWheel = oMouseData.MouseWheel;
}

void UIRenderer::SyncKeys(ImGuiIO& a_rImGuiIO)
{
	a_rImGuiIO.KeysDown[Input::E_TAB] = m_pInput->KeyDown(Input::E_TAB);
	a_rImGuiIO.KeysDown[Input::E_LEFT] = m_pInput->KeyDown(Input::E_LEFT);
	a_rImGuiIO.KeysDown[Input::E_RIGHT] = m_pInput->KeyDown(Input::E_RIGHT);
	a_rImGuiIO.KeysDown[Input::E_UP] = m_pInput->KeyDown(Input::E_UP);
	a_rImGuiIO.KeysDown[Input::E_DOWN] = m_pInput->KeyDown(Input::E_DOWN);
	a_rImGuiIO.KeysDown[Input::E_PAGE_UP] = m_pInput->KeyDown(Input::E_PAGE_UP);
	a_rImGuiIO.KeysDown[Input::E_PAGE_DOWN] = m_pInput->KeyDown(Input::E_PAGE_DOWN);
	a_rImGuiIO.KeysDown[Input::E_HOME] = m_pInput->KeyDown(Input::E_HOME);
	a_rImGuiIO.KeysDown[Input::E_END] = m_pInput->KeyDown(Input::E_END);
	a_rImGuiIO.KeysDown[Input::E_INSERT] = m_pInput->KeyDown(Input::E_INSERT);
	a_rImGuiIO.KeysDown[Input::E_DELETE] = m_pInput->KeyDown(Input::E_DELETE);
	a_rImGuiIO.KeysDown[Input::E_BACKSPACE] = m_pInput->KeyDown(Input::E_BACKSPACE);
	a_rImGuiIO.KeysDown[Input::E_SPACE] = m_pInput->KeyDown(Input::E_SPACE);
	a_rImGuiIO.KeysDown[Input::E_ENTER] = m_pInput->KeyDown(Input::E_ENTER);
	a_rImGuiIO.KeysDown[Input::E_ESC] = m_pInput->KeyDown(Input::E_ESC);
	a_rImGuiIO.KeysDown[Input::E_A] = m_pInput->KeyDown(Input::E_A);
	a_rImGuiIO.KeysDown[Input::E_C] = m_pInput->KeyDown(Input::E_C);
	a_rImGuiIO.KeysDown[Input::E_V] = m_pInput->KeyDown(Input::E_V);
	a_rImGuiIO.KeysDown[Input::E_X] = m_pInput->KeyDown(Input::E_X);
	a_rImGuiIO.KeysDown[Input::E_Y] = m_pInput->KeyDown(Input::E_Y);
	a_rImGuiIO.KeysDown[Input::E_Z] = m_pInput->KeyDown(Input::E_Z);

	a_rImGuiIO.KeysDown[Input::E_LEFT] = m_pInput->KeyDown(Input::E_LEFT);
	a_rImGuiIO.KeysDown[Input::E_RIGHT] = m_pInput->KeyDown(Input::E_RIGHT);
	a_rImGuiIO.KeysDown[Input::E_UP] = m_pInput->KeyDown(Input::E_UP);
	a_rImGuiIO.KeysDown[Input::E_DOWN] = m_pInput->KeyDown(Input::E_DOWN);

	a_rImGuiIO.KeyShift = m_pInput->KeyDown(Input::E_SHIFT);
	a_rImGuiIO.KeyCtrl = m_pInput->KeyDown(Input::E_LEFT_CTRL);
	a_rImGuiIO.KeyAlt = m_pInput->KeyDown(Input::E_ALT);

	const std::vector<char>& buffer = m_pInput->GetInputBuffer();
	for (const char& c : buffer)
		a_rImGuiIO.AddInputCharacter(c);
}
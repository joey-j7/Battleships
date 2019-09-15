#pragma once

#include "imgui.h"	
#include <vector>

#include <glm/glm.hpp>
#include "GLIncludes.h"

#include <map>

struct ImGuiIO;
struct ImDrawData;

class GameObject;
class Camera;
class Application;
class ShaderProgram;
class Texture;
class InputManager;
class GLContext;

class VBO;
class IBO;

namespace UIElements
{
	class UIElement;
	class LogWindow;
}

using namespace UIElements;

class UIRenderer
{
public:
	enum Layer
	{
		E_DEFAULT = 0,
		E_DEBUG   = 1,
		E_DIALOG = 2,
		E_EXIT_DIALOG = 3
	};

	UIRenderer();
	~UIRenderer();

	void Init();

	void InitFrame();
	void Render();

	static UIRenderer* GetInstance()
	{
		if (!m_pInstance)
			m_pInstance = new UIRenderer();

		return m_pInstance;
	}

	void Exit();
	
	ShaderProgram* GetShader() const { return m_pShader; }

	VBO* GetVBO() const { return m_pVBO; }
	IBO* GetIBO() const { return m_pIBO; }
  
	void RemoveElement(UIElement* a_pElement);
	void AddElement(Layer a_Layer, UIElement* a_pWindow);
	void ClearElements(bool a_bClearDebug = false);

	template <typename T>
	T* GetElement() const;

	GameObject* GetDebugObject() const { return m_pDebugObject; }
	void SetDebugObject(GameObject* a_pGameObject) { m_pDebugObject = a_pGameObject; }

private:
	void InitFont();
	void InitStyle();

	void MapKeys();

	void SyncMouse(ImGuiIO& a_rImGuiIO);
	void SyncKeys(ImGuiIO& a_rImGuiIO);

	void DeleteElement(UIElement* a_pElement);

	std::map<Layer, std::vector<UIElement*>> m_mLayers;
	std::vector<UIElement*> m_vDeleteQueue;

	/* Shaders */
	ShaderProgram* m_pShader = nullptr;

	VBO* m_pVBO;
	IBO* m_pIBO;

	Texture* m_pFontTexture = nullptr;
	GameObject* m_pDebugObject = nullptr;

#ifdef ANDROID
	bool m_bKeyboardShown;
#endif
	
	/* ImGui Requirements */
	Application* m_pApp = nullptr;
	GLContext* m_pGLContext = nullptr;

	InputManager* m_pInput = nullptr;
	static UIRenderer* m_pInstance;
};

template <typename T>
T* UIRenderer::GetElement() const
{
	for (auto it : m_mLayers)
	{
		for (UIElement* e : it.second)
		{
			T* c = dynamic_cast<T*>(e);
			if (c) return c;
		}
	}

	return nullptr;
}

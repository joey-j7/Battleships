#pragma once
#include "InputManager.h"
#include "UIRenderer.h"

namespace UIElements
{
	class UIElement
	{
	public:
		enum Type
		{
			E_WINDOW,
			E_MAINMENUBAR,
			E_DIALOG,
			E_FIXED_WINDOW 
		};

		UIElement(const std::string& a_sName, bool a_bHideClose = false, Type a_eType = E_WINDOW, UIRenderer::Layer a_eLayer = UIRenderer::E_DEFAULT, Input::KeyCode a_eToggleKey = Input::E_NONE);
		virtual ~UIElement() = default;

		void Destroy();

		virtual void Update();

		virtual bool Begin();
		virtual void Render() = 0;
		virtual void End();
		
		const std::string& GetName() const
		{
			return m_sName;
		}

		const Input::KeyCode& GetToggleKey() const
		{
			return m_eToggleKey;
		}

		bool IsActive() const
		{
			return m_bActive;
		}

		void SetPrevious(UIElement* a_pPrevious)
		{
			m_pPrevious = a_pPrevious;
		}

		virtual void SetActive(bool a_bActive);
		void AddChild(UIElement* a_pElement);

		UIElement* GetParent() const { return m_pParent; }

	protected:
		void EnablePrevious()
		{
			if (m_pPrevious) {
				SetActive(false);
				m_pPrevious->SetActive(true);
				m_pPrevious = nullptr;
			}
		}

		void SetParent(UIElement* a_pElement) { m_pParent = a_pElement; }

		bool m_bActive;
		bool m_bHideClose;

		Application* m_pApp = nullptr;

		UIElement* m_pParent = nullptr;
		std::vector<UIElement*> m_pChildren;

		std::string m_sName;
		Type m_eType;
		UIRenderer::Layer m_eLayer;
		Input::KeyCode m_eToggleKey;

		InputManager* m_pInput = nullptr;
		UIRenderer* m_pUIRenderer = nullptr;

		UIElement* m_pPrevious = nullptr;
	};
}

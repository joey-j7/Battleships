#pragma once

#include <vector>
#include <string>

#include <glm/glm.hpp>
#include "GLContext.h"

#ifdef RASPBERRY
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#elif ANDROID
#include "android_native_app_glue.h"
#elif WINDOWS
#include "GLIncludes.h"
#include <glfw3.h>
#endif

struct Input
{
	enum KeyState
	{
		E_KEY_UP       = 0,
		E_KEY_DOWN     = 1,
		E_KEY_PRESSED  = 2,
		E_KEY_RELEASED = 3
	};
	
	enum MouseButton
	{
#ifdef OPENGLES
    E_MOUSE_LEFT = 0,
    E_MOUSE_MIDDLE = 1,
    E_MOUSE_RIGHT = 2
#else
    E_MOUSE_LEFT = 0,
    E_MOUSE_MIDDLE = 2,
    E_MOUSE_RIGHT = 1
#endif
	};

	enum KeyCode
	{
#ifdef RASPBERRY
		E_NONE		 = 0,
		E_ESC        = 9,

		E_F1         = 67,
		E_F2         = 68,
		E_F3         = 69,
		E_F4         = 70,
		E_F5         = 71,
		E_F6         = 72,
		E_F7         = 73,
		E_F8         = 74,
		E_F9         = 75,
		E_F10        = 76,
		E_F11        = 95,
		E_F12        = 96,

		E_1          = 10,
		E_2          = 11,
		E_3          = 12,
		E_4          = 13,
		E_5          = 14,
		E_6          = 15,
		E_7          = 16,
		E_8          = 17,
		E_9          = 18,
		E_0          = 19,

		E_W          = 25,
		E_A          = 38,
		E_S          = 39,
		E_D          = 40,

		E_C          = 54,
		E_V          = 55,
		E_X          = 53,
		E_Y          = 29,
		E_Z          = 52,

		E_UP         = 111,
		E_DOWN       = 116,
		E_LEFT       = 113,
		E_RIGHT      = 114,
		
		E_SPACE      = 65,
		E_ENTER      = 36,
		E_SHIFT      = 50,
		E_ALT		 = 64,
		E_TAB        = 23,

		E_PAGE_UP = 112,
		E_PAGE_DOWN = 117,
		E_HOME = 110,
		E_END = 115,
		E_INSERT = 118,
		E_DELETE = 119,
		E_BACKSPACE = 22,
    
		E_DOT = 52,

		E_LEFT_CTRL  = 37,
		E_RIGHT_CTRL = 105,
#elif ANDROID
		E_NONE		 = 0,
		E_ESC        = 111,

		E_F1         = 131,
		E_F2         = 132,
		E_F3         = 133,
		E_F4         = 134,
		E_F5         = 135,
		E_F6         = 136,
		E_F7         = 137,
		E_F8         = 138,
		E_F9         = 139,
		E_F10        = 140,
		E_F11        = 141,
		E_F12        = 132,

		E_1          = 8,
		E_2          = 9,
		E_3          = 10,
		E_4          = 11,
		E_5          = 12,
		E_6          = 13,
		E_7          = 14,
		E_8          = 15,
		E_9          = 16,
		E_0          = 7,

		E_W          = 51,
		E_A          = 29,
		E_S          = 47,
		E_D          = 32,

		E_C          = 31,
		E_V          = 50,
		E_X          = 52,
		E_Y          = 53,
		E_Z          = 54,

		E_UP         = 19,
		E_DOWN       = 20,
		E_LEFT       = 21,
		E_RIGHT      = 22,
		
		E_SPACE      = 62,
		E_ENTER      = 66,
		E_SHIFT      = 59,
		E_ALT		 = 57,
		E_TAB        = 61,

		E_PAGE_UP = 92,
		E_PAGE_DOWN = 93,
		E_HOME = 3,
		E_END = 6,
		E_INSERT = 124,
		E_DELETE = 112,
		E_BACKSPACE = 67,
    
		E_DOT  = 56,

		E_LEFT_CTRL  = 113,
		E_RIGHT_CTRL = 114,
#elif WINDOWS
    E_NONE = 0,
    E_ESC = 256,

    E_F1 = 290,
    E_F2 = 291,
    E_F3 = 292,
    E_F4 = 293,
    E_F5 = 294,
    E_F6 = 295,
    E_F7 = 296,
    E_F8 = 297,
    E_F9 = 298,
    E_F10 = 299,
    E_F11 = 300,
    E_F12 = 301,

    E_1 = 49,
    E_2 = 50,
    E_3 = 51,
    E_4 = 52,
    E_5 = 53,
    E_6 = 54,
    E_7 = 55,
    E_8 = 56,
    E_9 = 57,
    E_0 = 48,

    E_W = 87,
    E_A = 65,
    E_S = 83,
    E_D = 68,

    E_C = 67,
    E_V = 86,
    E_X = 88,
    E_Y = 89,
    E_Z = 90,

    E_UP = 265,
    E_DOWN = 264,
    E_LEFT = 263,
    E_RIGHT = 262,

    E_SPACE = 32,
    E_ENTER = 257,
    E_SHIFT = 340,
    E_ALT = 342,
    E_TAB = 258,

    E_PAGE_UP = 266,
    E_PAGE_DOWN = 266,
    E_HOME = 268,
    E_END = 269,
    E_INSERT = 260,
    E_DELETE = 261,
    E_BACKSPACE = 259,
    
		E_DOT = 46,

    E_LEFT_CTRL = 341,
    E_RIGHT_CTRL = 345,
#endif
	};
};

class InputManager
{
  #ifdef RASPBERRY
	typedef XEvent Event;
  #elif WINDOWS
    typedef GLFWwindow Window;
    typedef int KeyCode;
    typedef int Event;
    typedef GLFWmonitor Display;
  #endif

public:
	InputManager(GLWindow* a_pGLWindow);
	
	bool Update();
	void Sync();

	static std::string KeycodeToString(Input::KeyCode a_eKey);
	
	// Getters for mouse values
	const glm::vec2& GetMouseVelocity() const;
	glm::vec2 GetMouseVelocityNorm() const;
	const glm::ivec2& GetMousePosition() const;
	int GetMouseWheelScrolled() const;
	
	/* Mouse */
	bool MouseUp(Input::MouseButton a_Button) const;
	bool MouseDown(Input::MouseButton a_Button) const;
	bool MousePressed(Input::MouseButton a_Button) const;
	bool MouseReleased(Input::MouseButton a_Button) const;
	
	bool MouseMoved(glm::vec2& a_iv2MouseVelocity) const;
	bool MouseWheelScrolled(int& a_iMouseScroll) const;

	/* Keys */
	bool KeyUp(Input::KeyCode a_Key) const;
	bool KeyDown(Input::KeyCode a_Key) const;
	bool KeyPressed(Input::KeyCode a_Key) const;
	bool KeyReleased(Input::KeyCode a_Key) const;

	void ActivateKey(Input::KeyCode a_Key);

	const std::vector<char>& GetInputBuffer() const { return m_vInputBuffer; }
	const Input::KeyState* GetKeyStates() const { return m_KeyState; }

private:
#ifdef RASPBERRY
	void OnMotionNotify(const Event& a_rEvent);
	void OnButtonPress(const Event& a_rEvent);
	void OnButtonRelease(const Event& a_rEvent);
	void OnKeyPress(Event& a_rEvent);
	void OnKeyRelease(const Event& a_rEvent);

	Display* m_pDisplay;
	static glm::ivec2 m_iv2MousePosition;
#elif ANDROID
public:
	static int32_t OnInput(android_app* app, AInputEvent* event);

	float GetMousePressDistance() const;
	float GetMouseMaxTapDistance() const { return m_fMouseMaxTapDistance; }
	float GetMousePinchOffsetNorm() const;

 	bool HasMultipleTouches() const { return m_bIsMultiTouch; }
    void ShowKeyboard(bool a_bShow);

	static float m_fTime;
	const float m_fMouseMaxTapDistance = 5.0f;

	static glm::ivec2 m_iv2MousePressPosition;
	static glm::ivec2 m_iv2MouseReleasePosition;

    static float m_fMousePinchOffset;
    static bool m_bIsMultiTouch;

	static glm::ivec2 m_iv2MousePosition[2];
private:

#elif WINDOWS
	static void OnKey(GLFWwindow* a_pWindow, int a_iKey, int a_iScancode, int a_iAction, int a_iMods);
	static void OnMouse(GLFWwindow* a_pWindow, int a_iKey, int a_iAction, int a_iMods);
	static void OnMousePos(GLFWwindow* a_pWindow, double a_dXPos, double a_dYPos);
	static void OnMouseScroll(GLFWwindow* a_pWindow, double a_dXOffset, double a_dYOffset);
    static void OnExit(GLFWwindow* a_pWindow);
  
	static glm::ivec2 m_iv2MousePosition;
#endif

	static Input::KeyState m_KeyState[512];
	static std::vector<char> m_vInputBuffer;

	static glm::vec2 m_v2MouseVelocity;

	static bool m_MouseMoved;

	static int m_iMouseScroll;
	static bool m_MouseScrolled;

	static Input::KeyState m_MouseState[3];
};

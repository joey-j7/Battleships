#include "InputManager.h"

#include "GLContext.h"
#include "GLWindow.h"

#include "Debug.h"
#include "ExitDialog.h"

#include "Application.h"

#ifdef ANDROID
#include "GameTime.h"

#include "engine.h"

float InputManager::m_fTime;

glm::ivec2 InputManager::m_iv2MousePressPosition;
glm::ivec2 InputManager::m_iv2MouseReleasePosition;

float InputManager::m_fMousePinchOffset;
glm::ivec2 InputManager::m_iv2MousePosition[2];

bool InputManager::m_bIsMultiTouch;
#else
glm::ivec2 InputManager::m_iv2MousePosition;
#endif

Input::KeyState InputManager::m_KeyState[512];
std::vector<char> InputManager::m_vInputBuffer;

glm::vec2 InputManager::m_v2MouseVelocity;

bool InputManager::m_MouseMoved;
int InputManager::m_iMouseScroll;
bool InputManager::m_MouseScrolled;

Input::KeyState InputManager::m_MouseState[3];

InputManager::InputManager(GLWindow* a_pGLWindow)
{
#ifdef RASPBERRY
	m_pDisplay = a_pGLWindow->GetDisplay();
	
	// Poll the current mouse position at the start
	// of the screen for prepartation of the first
	// mouse move event
	const int screen = DefaultScreen(m_pDisplay);
	unsigned int mask = 0;
	Window child_win, root_win;

	XQueryPointer(
    m_pDisplay,
		*a_pGLWindow->GetWindow(),
		&child_win,
		&root_win,
		&m_iv2MousePosition[0],
		&m_iv2MousePosition[1],
		&m_iv2MousePosition[0],
		&m_iv2MousePosition[1],
		&mask
	);
#elif WINDOWS
	glfwSetKeyCallback(a_pGLWindow->GetWindow(), OnKey);
	glfwSetMouseButtonCallback(a_pGLWindow->GetWindow(), OnMouse);
	glfwSetCursorPosCallback(a_pGLWindow->GetWindow(), OnMousePos);
	glfwSetScrollCallback(a_pGLWindow->GetWindow(), OnMouseScroll);
	glfwSetWindowCloseCallback(a_pGLWindow->GetWindow(), OnExit);

	glfwSetInputMode(a_pGLWindow->GetWindow(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
#endif
	
	// Default mouse moved set to false
	m_MouseMoved = false;
	
	// Default mouse scroll wheel
	m_iMouseScroll = 0;
	m_MouseScrolled = false;
}

bool InputManager::Update()
{
	// Do this to catch the change of 
	// mouse moved and mouse wheel scrolled
	m_MouseMoved = false;
	m_MouseScrolled = false;

#ifdef ANDROID
	m_fTime = GameTime::GetTime();
#else
	m_vInputBuffer.clear();
#endif

#ifdef RASPBERRY
	while (XPending(m_pDisplay)) 
	{
		// check for events from the x-server
		XEvent xev;
		XNextEvent(m_pDisplay, &xev);

		switch (xev.type)
		{
			/* Mouse Move */
		case MotionNotify:
			OnMotionNotify(xev);
			break;
		case ButtonPress:
			OnButtonPress(xev);
			break;
		case ButtonRelease:
			OnButtonRelease(xev);
			break;
		case KeyPress:
			OnKeyPress(xev);
			break;
		case KeyRelease:
			OnKeyRelease(xev);
			break;
		default: 
			break;
		}
	}
#elif WINDOWS
	glfwPollEvents();
#endif

	return true;
}

void InputManager::Sync()
{
	// Syncing of the key and mouse information after each update cycle

#ifdef ANDROID
	m_vInputBuffer.clear();
	
	/* Button release fix */
	for (int i = 0; i < 512; ++i)
		m_KeyState[i] = m_KeyState[i] == Input::E_KEY_PRESSED ? Input::E_KEY_RELEASED : m_KeyState[i];
#endif

	// Cycling and Syncing of the key states
	for (int i = 0; i < 512; ++i)
	{
		m_KeyState[i] = m_KeyState[i] == Input::E_KEY_PRESSED ? Input::E_KEY_DOWN : m_KeyState[i];
		m_KeyState[i] = m_KeyState[i] == Input::E_KEY_RELEASED ? Input::E_KEY_UP : m_KeyState[i];
	}

	// Cycling and Syncing of the mouse states
	for (int i = 0; i < 3; ++i)
	{
		m_MouseState[i] = m_MouseState[i] == Input::E_KEY_PRESSED ? Input::E_KEY_DOWN : m_MouseState[i];
		m_MouseState[i] = m_MouseState[i] == Input::E_KEY_RELEASED ? Input::E_KEY_UP : m_MouseState[i];
	}
	
	// Cycling and Syncing of the moved and mouse wheel scrolled state
	// False means it will reset the velocity to zero
	if(!m_MouseMoved)
	{
		m_v2MouseVelocity.x = 0;
		m_v2MouseVelocity.y = 0;
	}
	
	if (!m_MouseScrolled)
		m_iMouseScroll = 0;
}

std::string InputManager::KeycodeToString(Input::KeyCode a_eKey)
{
	if (a_eKey == Input::E_NONE) return "";
	if (a_eKey == Input::E_ESC) return "ESC";

	if (a_eKey == Input::E_F1) return "F1";
	if (a_eKey == Input::E_F2) return "F2";
	if (a_eKey == Input::E_F3) return "F3";
	if (a_eKey == Input::E_F4) return "F4";
	if (a_eKey == Input::E_F5) return "F5";
	if (a_eKey == Input::E_F6) return "F6";
	if (a_eKey == Input::E_F7) return "F7";
	if (a_eKey == Input::E_F8) return "F8";
	if (a_eKey == Input::E_F9) return "F9";
	if (a_eKey == Input::E_F10) return "F10";
	if (a_eKey == Input::E_F11) return "F11";
	if (a_eKey == Input::E_F12) return "F12";
  
	if (a_eKey == Input::E_1) return "1";
	if (a_eKey == Input::E_2) return "2";
	if (a_eKey == Input::E_3) return "3";
	if (a_eKey == Input::E_4) return "4";
	if (a_eKey == Input::E_5) return "5";
	if (a_eKey == Input::E_6) return "6";
	if (a_eKey == Input::E_7) return "7";
	if (a_eKey == Input::E_8) return "8";
	if (a_eKey == Input::E_9) return "9";
	if (a_eKey == Input::E_0) return "0";

	if (a_eKey == Input::E_W) return "W";
	if (a_eKey == Input::E_A) return "A";
	if (a_eKey == Input::E_S) return "S";
	if (a_eKey == Input::E_D) return "D";
  
	if (a_eKey == Input::E_C) return "C";
	if (a_eKey == Input::E_V) return "V";
	if (a_eKey == Input::E_X) return "X";
	if (a_eKey == Input::E_Y) return "Y";
	if (a_eKey == Input::E_Z) return "Z";
  
	if (a_eKey == Input::E_UP) return "Up";
	if (a_eKey == Input::E_DOWN) return "Down";
	if (a_eKey == Input::E_LEFT) return "Left";
	if (a_eKey == Input::E_RIGHT) return "Right";
  
	if (a_eKey == Input::E_SPACE) return "Space";
	if (a_eKey == Input::E_ENTER) return "Enter";
	if (a_eKey == Input::E_SHIFT) return "Shift";
	if (a_eKey == Input::E_ALT) return "Alt";
	if (a_eKey == Input::E_TAB) return "Tab";
  
	if (a_eKey == Input::E_PAGE_UP) return "Page Up";
	if (a_eKey == Input::E_PAGE_DOWN) return "Page Down";
	if (a_eKey == Input::E_HOME) return "Home";
	if (a_eKey == Input::E_END) return "End";
	if (a_eKey == Input::E_INSERT) return "Insert";
	if (a_eKey == Input::E_DELETE) return "Delete";
	if (a_eKey == Input::E_BACKSPACE) return "Backspace";
  
	if (a_eKey == Input::E_DOT) return ".";
		
	if (a_eKey == Input::E_LEFT_CTRL) return "Left CTRL";
	if (a_eKey == Input::E_RIGHT_CTRL) return "Right CTRL";

	return "";
}

// Getters for mouse values
const glm::ivec2& InputManager::GetMousePosition() const
{
#ifdef ANDROID
	return m_iv2MousePosition[0];
#else
	return m_iv2MousePosition;
#endif
}

const glm::vec2& InputManager::GetMouseVelocity() const
{
	return m_v2MouseVelocity;
}

glm::vec2 InputManager::GetMouseVelocityNorm() const
{
  const glm::vec2 v2ScreenSizeInv = Application::GetInstance()->GetGLWindow()->GetSizeInv();
  return m_v2MouseVelocity * glm::max(v2ScreenSizeInv.x, v2ScreenSizeInv.y);
}

int InputManager::GetMouseWheelScrolled() const
{
	return m_iMouseScroll;
}

/* Mouse */
bool InputManager::MouseUp(Input::MouseButton a_Button) const
{
	return m_MouseState[a_Button] == Input::E_KEY_RELEASED || m_MouseState[a_Button] == Input::E_KEY_UP;
}

bool InputManager::MouseDown(Input::MouseButton a_Button) const
{
	return m_MouseState[a_Button] == Input::E_KEY_PRESSED || m_MouseState[a_Button] == Input::E_KEY_DOWN;
}

bool InputManager::MouseReleased(Input::MouseButton a_Button) const
{
	return m_MouseState[a_Button] == Input::E_KEY_RELEASED;
}

#ifndef ANDROID
bool InputManager::MousePressed(Input::MouseButton a_Button) const
{
	return m_MouseState[a_Button] == Input::E_KEY_PRESSED;
}
#elif ANDROID
bool InputManager::MousePressed(Input::MouseButton a_Button) const
{
	return MouseReleased(a_Button) && GetMousePressDistance() < m_fMouseMaxTapDistance;
}

float InputManager::GetMousePressDistance() const
{
	return glm::distance((glm::vec2)m_iv2MouseReleasePosition, (glm::vec2)m_iv2MousePressPosition);
}

float InputManager::GetMousePinchOffsetNorm() const
{
	const glm::vec2 v2ScreenSizeInv = Application::GetInstance()->GetGLWindow()->GetSizeInv();
	return m_fMousePinchOffset * glm::max(v2ScreenSizeInv.x, v2ScreenSizeInv.y);
}

void InputManager::ShowKeyboard(bool a_bShow) {
    android_app* app = Application::GetInstance()->GetGLContext()->GetEngine()->app;
    // Attaches the current thread to the JVM.
    jint lResult;
    jint lFlags = 0;

    JavaVM* lJavaVM = app->activity->vm;
    JNIEnv* lJNIEnv = app->activity->env;

    JavaVMAttachArgs lJavaVMAttachArgs;
    lJavaVMAttachArgs.version = JNI_VERSION_1_6;
    lJavaVMAttachArgs.name = "NativeThread";
    lJavaVMAttachArgs.group = NULL;

    lResult=lJavaVM->AttachCurrentThread(&lJNIEnv, &lJavaVMAttachArgs);
    if (lResult == JNI_ERR) {
        return;
    }

    // Retrieves NativeActivity.
    jobject lNativeActivity = app->activity->clazz;
    jclass ClassNativeActivity = lJNIEnv->GetObjectClass(lNativeActivity);

    // Retrieves Context.INPUT_METHOD_SERVICE.
    jclass ClassContext = lJNIEnv->FindClass("android/content/Context");
    jfieldID FieldINPUT_METHOD_SERVICE =
        lJNIEnv->GetStaticFieldID(ClassContext,
            "INPUT_METHOD_SERVICE", "Ljava/lang/String;");
    jobject INPUT_METHOD_SERVICE =
        lJNIEnv->GetStaticObjectField(ClassContext,
            FieldINPUT_METHOD_SERVICE);
    //jniCheck(INPUT_METHOD_SERVICE);

    // Runs getSystemService(Context.INPUT_METHOD_SERVICE).
    jclass ClassInputMethodManager = lJNIEnv->FindClass(
        "android/view/inputmethod/InputMethodManager");
    jmethodID MethodGetSystemService = lJNIEnv->GetMethodID(
        ClassNativeActivity, "getSystemService",
        "(Ljava/lang/String;)Ljava/lang/Object;");
    jobject lInputMethodManager = lJNIEnv->CallObjectMethod(
        lNativeActivity, MethodGetSystemService,
        INPUT_METHOD_SERVICE);

    // Runs getWindow().getDecorView().
    jmethodID MethodGetWindow = lJNIEnv->GetMethodID(
        ClassNativeActivity, "getWindow",
        "()Landroid/view/Window;");
    jobject lWindow = lJNIEnv->CallObjectMethod(lNativeActivity,
        MethodGetWindow);
    jclass ClassWindow = lJNIEnv->FindClass(
        "android/view/Window");
    jmethodID MethodGetDecorView = lJNIEnv->GetMethodID(
        ClassWindow, "getDecorView", "()Landroid/view/View;");
    jobject lDecorView = lJNIEnv->CallObjectMethod(lWindow,
        MethodGetDecorView);

    if (a_bShow) {
        // Runs lInputMethodManager.showSoftInput(...).
        jmethodID MethodShowSoftInput = lJNIEnv->GetMethodID(
            ClassInputMethodManager, "showSoftInput",
            "(Landroid/view/View;I)Z");
        jboolean lResult = lJNIEnv->CallBooleanMethod(
            lInputMethodManager, MethodShowSoftInput,
            lDecorView, lFlags);
    } else {
        // Runs lWindow.getViewToken()
        jclass ClassView = lJNIEnv->FindClass(
            "android/view/View");
        jmethodID MethodGetWindowToken = lJNIEnv->GetMethodID(
            ClassView, "getWindowToken", "()Landroid/os/IBinder;");
        jobject lBinder = lJNIEnv->CallObjectMethod(lDecorView,
            MethodGetWindowToken);

        // lInputMethodManager.hideSoftInput(...).
        jmethodID MethodHideSoftInput = lJNIEnv->GetMethodID(
            ClassInputMethodManager, "hideSoftInputFromWindow",
            "(Landroid/os/IBinder;I)Z");
        jboolean lRes = lJNIEnv->CallBooleanMethod(
            lInputMethodManager, MethodHideSoftInput,
            lBinder, lFlags);
    }

    // Finished with the JVM.
    lJavaVM->DetachCurrentThread();
}
#endif

void InputManager::ActivateKey(Input::KeyCode a_Key)
{
  m_KeyState[a_Key] = Input::E_KEY_PRESSED;
}

bool InputManager::MouseMoved(glm::vec2& a_v2MouseVelocity) const
{
	a_v2MouseVelocity = GetMouseVelocity();
	return m_MouseMoved;
}

bool InputManager::MouseWheelScrolled(int& a_iMouseScroll) const
{
	a_iMouseScroll = GetMouseWheelScrolled();
	return m_MouseScrolled;
}

/* Keys */
bool InputManager::KeyUp(Input::KeyCode a_Key) const
{
	return m_KeyState[a_Key] == Input::E_KEY_RELEASED || m_KeyState[a_Key] == Input::E_KEY_UP;
}

bool InputManager::KeyDown(Input::KeyCode a_Key) const
{
	return m_KeyState[a_Key] == Input::E_KEY_PRESSED || m_KeyState[a_Key] == Input::E_KEY_DOWN;
}

bool InputManager::KeyPressed(Input::KeyCode a_Key) const
{
	return m_KeyState[a_Key] == Input::E_KEY_PRESSED;
}

bool InputManager::KeyReleased(Input::KeyCode a_Key) const
{
	return m_KeyState[a_Key] == Input::E_KEY_RELEASED;
}

#ifdef RASPBERRY
void InputManager::OnMotionNotify(const Event& a_rEvent)
{
	m_v2MouseVelocity.x = -(m_iv2MousePosition.x - a_rEvent.xbutton.x);
	m_v2MouseVelocity.y = m_iv2MousePosition.y - a_rEvent.xbutton.y;
			
	m_iv2MousePosition.x = a_rEvent.xbutton.x;
	m_iv2MousePosition.y = a_rEvent.xbutton.y;
			
	m_MouseMoved = true;
}

void InputManager::OnButtonPress(const Event& a_rEvent)
{
	// Switch statement for catching mouse scroll event
	// Button4(Up) and Button5(Down) for mouse wheel scroll
	// Default for normal mouse button event handling
	switch(a_rEvent.xbutton.button)
	{
	case Button4:
		++m_iMouseScroll;
		m_MouseScrolled = true;
		break;
	case Button5:
		--m_iMouseScroll;
		m_MouseScrolled = true;
		break;
	default:
		m_MouseState[a_rEvent.xbutton.button - 1] = Input::E_KEY_PRESSED;
		break;
	}
}

void InputManager::OnButtonRelease(const Event& a_rEvent)
{
	m_MouseState[a_rEvent.xbutton.button - 1] = Input::E_KEY_RELEASED;
}

void InputManager::OnKeyPress(Event& a_rEvent)
{
	char cBuff;
	KeySym ks;
	XComposeStatus comp;

	XLookupString(&a_rEvent.xkey, &cBuff, 1, &ks, &comp);
	m_vInputBuffer.push_back(cBuff);

	m_KeyState[a_rEvent.xkey.keycode] = Input::E_KEY_PRESSED;
}

void InputManager::OnKeyRelease(const Event& a_rEvent)
{
	m_KeyState[a_rEvent.xkey.keycode] = Input::E_KEY_RELEASED;
}

#elif ANDROID
int32_t InputManager::OnInput(android_app* app, AInputEvent* event)
{
	struct engine* engine = (struct engine*)app->userData;
	int32_t iEventType = AInputEvent_getType(event);
	
	if (iEventType == AINPUT_EVENT_TYPE_MOTION) {
		const int32_t action = AMotionEvent_getAction(event);
		const size_t touchCount = AMotionEvent_getPointerCount(event);
		
		saved_state& engineState = engine->state;
		
		engineState.x = AMotionEvent_getX(event, 0);
		engineState.y = AMotionEvent_getY(event, 0);
		
		if (touchCount > 1)
		{
			glm::ivec2 iv2MousePosition0(engineState.x, engineState.y);
			const glm::ivec2 iv2MousePosition1(AMotionEvent_getX(event, 1), AMotionEvent_getY(event, 1));
			
			if (!m_bIsMultiTouch) {
				iv2MousePosition0 = m_iv2MousePosition[0];
				m_iv2MousePosition[1] = iv2MousePosition1;
			
				m_bIsMultiTouch = true;
			}
			
			const float fDist1 = glm::distance((glm::vec2)m_iv2MousePosition[0], (glm::vec2)m_iv2MousePosition[1]);
			const float fDist2 = glm::distance((glm::vec2)iv2MousePosition0, (glm::vec2)iv2MousePosition1);
			m_fMousePinchOffset = fDist2 - fDist1;
			
			m_iv2MousePosition[1] = iv2MousePosition1;
		}
		else if (m_bIsMultiTouch) {
			m_fMousePinchOffset = 0;
			
			/* When finger 1 is released, finger 2 becomes finger 1 */
			const glm::ivec2 iv2MousePosition0(engineState.x, engineState.y);
			
			const float fDist1 = glm::distance((glm::vec2)iv2MousePosition0, (glm::vec2)m_iv2MousePosition[0]);
			const float fDist2 = glm::distance((glm::vec2)iv2MousePosition0, (glm::vec2)m_iv2MousePosition[1]);
			const bool bTakeOver = fDist1 > fDist2;
			
			if (bTakeOver) {
				engineState.x = m_iv2MousePosition[1].x;
				engineState.y = m_iv2MousePosition[1].y;
				m_iv2MousePosition[0] = m_iv2MousePosition[1];
			}
			
			m_bIsMultiTouch = false;
		}
		
		if (action == AMOTION_EVENT_ACTION_DOWN)
		{
			m_iv2MousePressPosition.x = engineState.x;
			m_iv2MousePressPosition.y = engineState.y;
			
			m_MouseState[Input::E_MOUSE_LEFT] = Input::E_KEY_PRESSED;
		}
		else if (action == AMOTION_EVENT_ACTION_UP)
		{
			m_iv2MouseReleasePosition.x = engineState.x;
			m_iv2MouseReleasePosition.y = engineState.y;
			
			m_MouseState[Input::E_MOUSE_LEFT] = Input::E_KEY_RELEASED;
		}
		else if (action == AMOTION_EVENT_ACTION_MOVE)
		{
			m_v2MouseVelocity.x = -(m_iv2MousePosition[0].x - engineState.x);
			m_v2MouseVelocity.y = m_iv2MousePosition[0].y - engineState.y;
		
			m_MouseMoved = true;
		}
		
		m_iv2MousePosition[0] = glm::ivec2(engineState.x, engineState.y);
		
		return 1;
	}
	else if (iEventType == AINPUT_EVENT_TYPE_KEY) {
		const int32_t action = AKeyEvent_getAction(event);
	
		int32_t key = AKeyEvent_getKeyCode(event);
	    
		if (key < 0 || key > 511)
			return 0;
	
		if (action == AKEY_EVENT_ACTION_DOWN)
		{
			std::string keyString = KeycodeToString((Input::KeyCode)key);
			m_KeyState[key] = Input::E_KEY_PRESSED;
	
			if (keyString.size() == 1)
				m_vInputBuffer.push_back(keyString[0]);
		}
		else if (action == AKEY_EVENT_ACTION_MULTIPLE)
			m_KeyState[key] = Input::E_KEY_DOWN;
		//else if (action == AKEY_EVENT_ACTION_UP)
		//	m_KeyState[key] = Input::E_KEY_RELEASED;
		//else
		//	m_KeyState[key] = Input::E_KEY_UP;
	}
	
	return 0;
}

#elif WINDOWS

void InputManager::OnKey(GLFWwindow* a_pWindow, int a_iKey, int a_iScancode, int a_iAction, int a_iMods)
{
	if (a_iKey < 0 || a_iKey > 511)
		return;

	if (a_iAction == GLFW_PRESS)
	{
		const char* cBuff = glfwGetKeyName(a_iKey, a_iScancode);
		if (cBuff) m_vInputBuffer.push_back(*cBuff);

		m_KeyState[a_iKey] = Input::E_KEY_PRESSED;
	}
	else if (a_iAction == GLFW_RELEASE)
		m_KeyState[a_iKey] = Input::E_KEY_RELEASED;
	else if (a_iAction == GLFW_REPEAT)
		m_KeyState[a_iKey] = Input::E_KEY_DOWN;
	else
		m_KeyState[a_iKey] = Input::E_KEY_UP;
}

void InputManager::OnMouse(GLFWwindow* a_pWindow, int a_iKey, int a_iAction, int a_iMods)
{
	if (a_iKey < 0)
		return;

	if (a_iAction == GLFW_PRESS)
		m_MouseState[a_iKey] = Input::E_KEY_PRESSED;
	else if (a_iAction == GLFW_RELEASE)
		m_MouseState[a_iKey] = Input::E_KEY_RELEASED;
	else if (a_iAction == GLFW_REPEAT)
		m_MouseState[a_iKey] = Input::E_KEY_DOWN;
	else
		m_MouseState[a_iKey] = Input::E_KEY_UP;
}

void InputManager::OnMousePos(GLFWwindow* a_pWindow, double a_dXPos, double a_dYPos)
{
	m_v2MouseVelocity.x = -(m_iv2MousePosition.x - a_dXPos);
	m_v2MouseVelocity.y = m_iv2MousePosition.y - a_dYPos;

	m_iv2MousePosition.x = a_dXPos;
	m_iv2MousePosition.y = a_dYPos;

	m_MouseMoved = true;
}

void InputManager::OnMouseScroll(GLFWwindow* a_pWindow, double a_dXOffset, double a_dYOffset)
{
	m_iMouseScroll = a_dYOffset;
	m_MouseScrolled = true;
}

void InputManager::OnExit(GLFWwindow* a_pWindow)
{
  ExitDialog::Activate();
}

#endif

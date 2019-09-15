#pragma once

#ifdef DEBUG
#include "imgui.h"
#include <cstdarg>
#endif

#include <string>
#include <cstring>

class Debug
{
public:
	static void Log(const char* a_pcMessage, bool a_bLogToWindow = true, ...);
	static void Log(std::string a_sMessage, bool a_bLogToWindow = true, ...);

#ifdef DEBUG
	static ImGuiTextBuffer LogTextBuffer;

private:
	static std::string m_sLastMessage;
	static unsigned int m_uiCount;
#endif
};

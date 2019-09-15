#include "Debug.h"
#include "GameTime.h"

#include "Utils.h"

#ifdef WINDOWS
#include <windows.h>
#include <debugapi.h>
#elif ANDROID

#include <android/log.h>

#endif

#ifdef DEBUG
ImGuiTextBuffer Debug::LogTextBuffer;
std::string Debug::m_sLastMessage;
unsigned int Debug::m_uiCount;
#endif

void Debug::Log(const char* a_pcMessage, bool a_bLogToWindow, ...)
{
#ifdef DEBUG
	if (!a_bLogToWindow)
	{
		printf("%s\n", a_pcMessage);
		return;
	}

	std::string sMessage = a_pcMessage;
	if (sMessage != m_sLastMessage)
	{
		m_sLastMessage = sMessage;
		m_uiCount = 1;
	}
	else {
		m_uiCount++;

		const std::string sString = (" [" + to_string(m_uiCount) + "]\n");

		if (m_uiCount == 2)
		{
			LogTextBuffer.Buf.resize(LogTextBuffer.Buf.Size - 1);
			LogTextBuffer.Buf.reserve(LogTextBuffer.Buf.Size + sString.size());

			va_list vaArgs;
			va_start(vaArgs, a_bLogToWindow);
			LogTextBuffer.appendfv(sString.c_str(), vaArgs);
			va_end(vaArgs);
		}
		else
		{
			const std::string sOldString = (" [" + to_string(m_uiCount - 1) + "]\n");

			LogTextBuffer.Buf.resize(LogTextBuffer.Buf.Size - sOldString.size());
			LogTextBuffer.Buf.reserve(LogTextBuffer.Buf.Size + sString.size());
		
			va_list vaArgs;
			va_start(vaArgs, a_bLogToWindow);
			LogTextBuffer.appendfv(sString.c_str(), vaArgs);
			va_end(vaArgs);
		}

		printf("%s\n", a_pcMessage);
		return;
	}
	
	sMessage = "[" + GameTime::GetTimeString() + "] " + sMessage;
	sMessage += "\n";

	va_list vaArgs;
	va_start(vaArgs, a_bLogToWindow);
	LogTextBuffer.appendfv(sMessage.c_str(), vaArgs);
	va_end(vaArgs);

#ifdef RASPBERRY
  printf("%s\n", a_pcMessage);
#elif ANDROID
  __android_log_print(ANDROID_LOG_INFO , "Framework", a_pcMessage);
#elif WINDOWS
  OutputDebugString(a_pcMessage);
  OutputDebugString("\n");
#endif

#endif
}

void Debug::Log(std::string a_sMessage, bool a_bLogToWindow, ...)
{
#ifdef DEBUG
	Log(a_sMessage.c_str(), a_bLogToWindow);
#endif
}

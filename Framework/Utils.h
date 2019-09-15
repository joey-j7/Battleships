#pragma once

#include <string>
#include <sstream>
#include <glm/glm.hpp>

#ifdef EGL
#include <EGL/egl.h>
#endif

template <typename T>
std::string to_string(T value)
{
	std::ostringstream os;
	os << value;
	return os.str();
}

#ifdef DEBUG

#include "GLIncludes.h"
#include "Debug.h"

#define GL_GET_ERROR()                                              \
{																	                                  \
    GLenum error;                                                   \
    bool err = false;                                               \
    while ( (error = glGetError()) != GL_NO_ERROR)                  \
    {                                                               \
        Debug::Log("OpenGL ERROR:" + std::string(GLErrorToString(error))); \
        err = true;                                                 \
    }                                                               \
}

#ifdef ANDROID

#include "GLContext.h"

#define EGL_GET_ERROR()																			\
{																								\
    EGLint error;																				\
	while ((error = eglGetError()) != EGL_SUCCESS) {											\
		switch (error) {																		\
			case EGL_CONTEXT_LOST:																\
				Debug::Log("NativeEngine: egl error: EGL_CONTEXT_LOST. Recreating context.");	\
				GLContext::KillContext();														\
			case EGL_BAD_CONTEXT:																\
				Debug::Log("NativeEngine: egl error: EGL_BAD_CONTEXT. Recreating context.");	\
				GLContext::KillContext();														\
			case EGL_BAD_DISPLAY:																\
				Debug::Log("NativeEngine: egl error: EGL_BAD_DISPLAY. Recreating display.");	\
				GLContext::KillDisplay();														\
			case EGL_BAD_SURFACE:																\
				Debug::Log("NativeEngine: egl error: EGL_BAD_SURFACE. Recreating display.");	\
				GLContext::KillSurface();														\
			default:																			\
				Debug::Log("NativeEngine: unknown egl error: " + to_string(error));				\
		}																						\
	}																							\
}																								\

#endif

const char* GLErrorToString(GLenum a_err);
#else
#define GL_GET_ERROR()

#ifdef ANDROID
#define EGL_GET_ERROR()
#endif
#endif

namespace igad
{
	const float DEG2RAD = 0.0174532925f;
	const float RAD2DEG = 57.2957795f;
	const float PI = 3.14159265359f;
}

std::string LoadTextFile(const char* a_filePath);
char* LoadFile(const char* a_filePath, int& a_iSize);
std::string CacheFileToPath(const char* a_filePath);

std::string GetPlatformPath(const char* a_filePath);
std::string GetPlatformPath(std::string a_filePath);

std::string GetFolderPath(const std::string& a_sFilePath);
std::string GetFileName(const std::string& a_sFilePath);

inline std::string GetBaseDir(const std::string& filepath) 
{
	if (filepath.find_last_of("/\\") != std::string::npos)
		return filepath.substr(0, filepath.find_last_of("/\\"));
	return "";
}

struct Matrix4
{
	static glm::vec3 GetEulerAngles(const glm::mat4& a_mat4);
	static glm::vec3 GetEulerAngles(const glm::mat4& a_mat4, const glm::vec3& a_v3Scale);
	static glm::vec3 GetScale(const glm::mat4& a_mat4);
	static glm::vec3 GetTranslation(const glm::mat4& a_mat4);
};

struct Random
{
	static float InRange(float a_fMin, float a_fMax);
	static int InRange(int a_iMin, int a_iMax);
};
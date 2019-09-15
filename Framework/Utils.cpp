#include "Utils.h"
#include <cstring>
#include <fstream>
#include <cassert>
#include <cmath>
#include <glm/gtx/compatibility.hpp>
#include "GLIncludes.h"
#include "Application.h"

#ifdef ANDROID
#include "engine.h"
#endif

std::string LoadTextFile(const char* a_filePath)
{
#ifdef ANDROID
  engine* e = Application::GetInstance()->GetGLContext()->GetEngine();

  /* Disregard 7 characters: 'Assets/' */
  AAsset* asset = AAssetManager_open(e->assetManager, a_filePath + 7, AASSET_MODE_STREAMING);

  if (asset)
  {
    const size_t fileLength = AAsset_getLength(asset);

    std::string buf;
    buf.resize(fileLength);

    AAsset_read(asset, (void*)buf.data(), fileLength);
    AAsset_close(asset);

    return buf;
  }

  return "";
#else
  std::string line = "";

  std::ifstream textFile(a_filePath, std::ios::binary | std::ios::ate);
  if (textFile.is_open())
  {
	  const std::streamsize size = textFile.tellg();
	  line.resize(size);

	  textFile.seekg(0, std::ios::beg);
	  textFile.read((char*)line.data(), size);
  }
  return line;
#endif
}

char* LoadFile(const char* a_filePath, int& a_iSize)
{
#ifdef ANDROID
	engine* e = Application::GetInstance()->GetGLContext()->GetEngine();

	/* Disregard 7 characters: 'Assets/' */
	AAsset* asset = AAssetManager_open(e->assetManager, a_filePath + 7, AASSET_MODE_STREAMING);

	if (asset)
	{
		a_iSize = AAsset_getLength(asset);
		if (a_iSize <= 0) {
			Debug::Log("Asset " + std::string(a_filePath) + " has a length of zero");
			return nullptr;
		}

		char* c;
		c = new char[a_iSize];

		AAsset_read(asset, (void*)c, a_iSize);
		AAsset_close(asset);

		return c;
	}
	else
		Debug::Log("Asset " + std::string(a_filePath) + " not found");

	return nullptr;
#else
  std::ifstream textFile(a_filePath, std::ios::binary | std::ios::ate);

  a_iSize = textFile.tellg();
  if (a_iSize <= 0) return nullptr;

  char* c = new char[a_iSize];

  textFile.seekg(0, std::ios::beg);
  textFile.read(c, a_iSize);

  return c;
#endif
}

#ifdef ANDROID
#include <cstdlib>
#endif

std::string CacheFileToPath(const char* a_filePath)
{
#ifdef ANDROID
  engine* e = Application::GetInstance()->GetGLContext()->GetEngine();
  const std::string cachePath = std::string(e->cachePath) + "/";

  /* Disregard 7 characters: 'Assets/' */
  AAsset* asset = AAssetManager_open(e->assetManager, a_filePath + 7, AASSET_MODE_STREAMING);

  if (asset)
  {
    int iSize = AAsset_getLength(asset);
    if (iSize <= 0) return "";

    char* c;
    c = new char[iSize];

    int nb_read = 0;

	  /* Create directory structure */
	std::string cacheFilePath = cachePath + a_filePath;
    const char* cacheFolderPath = GetFolderPath(cacheFilePath).c_str();
    
	const int dir_err = system(("mkdir -p " + std::string(cacheFolderPath)).c_str());
	if (-1 == dir_err)
	{
		printf("Error creating directory!n");
		exit(1);
	}

	/* Make spaces compatible */
	std::replace(cacheFilePath.begin(), cacheFilePath.end(), '/', '\\');

	/* Create file in file path */
    FILE* out = fopen(cacheFilePath.c_str(), "w");

    while ((nb_read = AAsset_read(asset, (void*)c, iSize)) > 0)
		fwrite(c, nb_read, 1, out);

    fclose(out);

    AAsset_read(asset, (void*)c, iSize);
    AAsset_close(asset);

    return cacheFilePath;
  }

  return "";
#else
  Debug::Log("Warning: File cacheing not required for this platform, ignored");
  return "";
#endif
}

std::string GetPlatformPath(const char* a_filePath)
{
#ifdef RASPBERRY
	return std::string("../" + std::string(a_filePath));
#elif ANDROID
  return std::string(a_filePath);
#elif WINDOWS
	return std::string("./" + std::string(a_filePath));
#endif
}

std::string GetPlatformPath(std::string a_filePath)
{
	return GetPlatformPath(a_filePath.c_str());
}

const char* GLErrorToString(GLenum a_err)
{
	switch (a_err)
	{
	case GL_NO_ERROR:                      return "GL_NO_ERROR";
	case GL_INVALID_ENUM:                  return "GL_INVALID_ENUM";
	case GL_INVALID_VALUE:                 return "GL_INVALID_VALUE";
	case GL_INVALID_OPERATION:             return "GL_INVALID_OPERATION";
	case GL_OUT_OF_MEMORY:                 return "GL_OUT_OF_MEMORY";
	default:
		assert(!"Unhandled GL error code");
		return "";
	}
}

std::string GetFolderPath(const std::string& a_sFilePath)
{
	std::string sFolderPath;

	size_t last_slash_idx = a_sFilePath.rfind('/');

	if (std::string::npos != last_slash_idx)
		sFolderPath = a_sFilePath.substr(0, last_slash_idx) + "/";
	else
	{
		last_slash_idx = a_sFilePath.rfind('\\');
	
		if (std::string::npos != last_slash_idx)
			sFolderPath = a_sFilePath.substr(0, last_slash_idx) + "\\";
	}

	return sFolderPath;
}

std::string GetFileName(const std::string& a_sFilePath)
{
	std::string sFileName = "";

	const size_t last_slash_idx = a_sFilePath.rfind('/');
	const size_t last_dot_idx = a_sFilePath.rfind('.');

	if (std::string::npos != last_slash_idx)
		sFileName = a_sFilePath.substr(last_slash_idx + 1, last_dot_idx - last_slash_idx - 1);

	return sFileName;
}

glm::vec3 Matrix4::GetScale(const glm::mat4& a_mat4)
{
	float x = glm::length(glm::vec3(a_mat4[0][0], a_mat4[1][0], a_mat4[2][0]));
	float y = glm::length(glm::vec3(a_mat4[0][1], a_mat4[1][1], a_mat4[2][1]));
	float z = glm::length(glm::vec3(a_mat4[0][2], a_mat4[1][2], a_mat4[2][2]));
	return glm::vec3(x, y, z);
}

glm::vec3 Matrix4::GetTranslation(const glm::mat4& a_mat4)
{
	return glm::vec3(a_mat4[3][0], a_mat4[3][1], a_mat4[3][2]);
}

glm::vec3 Matrix4::GetEulerAngles(const glm::mat4& a_mat4)
{
	glm::vec3 scale = GetScale(a_mat4);
	return GetEulerAngles(a_mat4, scale);
}

glm::vec3 Matrix4::GetEulerAngles(const glm::mat4& a_mat4, const glm::vec3& a_v3Scale)
{
	glm::vec3 euler;
	euler.x = glm::asin(-a_mat4[2][1] / a_v3Scale.y);
	if (glm::cos(euler.x) > 0.0001)
	{
		euler.y = std::atan2(a_mat4[2][0] / a_v3Scale.x, a_mat4[2][2] / a_v3Scale.z) * igad::RAD2DEG;
		euler.z = std::atan2(a_mat4[0][1] / a_v3Scale.y, a_mat4[1][1] / a_v3Scale.y)  * igad::RAD2DEG;
	}
	else
	{
		euler.y = 0.0f;
		euler.z = std::atan2(a_mat4[1][0] / a_v3Scale.x, a_mat4[0][0] / a_v3Scale.x) * igad::RAD2DEG;
	}

	euler.x *= igad::RAD2DEG;
	return euler;
}

float Random::InRange(float a_fMin, float a_fMax)
{
	return a_fMin + static_cast<float>(rand()) / static_cast<float>(RAND_MAX / (a_fMax - a_fMin));
}

int Random::InRange(int a_iMin, int a_iMax)
{
	return a_iMin + (rand() % (a_iMax - a_iMin + 1));
}
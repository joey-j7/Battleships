#pragma once

#ifdef RASPBERRY
#include <GLES2/gl2.h>
#elif ANDROID
#include <GLES2/gl2.h>
#elif APPLE
#include <OpenGLES/ES1/gl.h>
#elif WINDOWS
#include <glad/glad.h>
#endif
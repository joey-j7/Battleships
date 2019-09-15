#pragma once

#include "pch.h"
#include <string>

class Game;

/**
* Our saved state data.
*/
struct saved_state {
  float angle;
  int32_t x;
  int32_t y;
};

/**
* Shared state for our app.
*/
struct engine {
	Game* game;
	struct android_app* app;
	
	const char* appPath;
	const char* cachePath;
	
	AAssetManager* assetManager;
	
	ASensorManager* sensorManager;
	const ASensor* accelerometerSensor;
	ASensorEventQueue* sensorEventQueue;
	
	int animating;
	EGLDisplay display;
	EGLSurface surface;
	EGLContext context;
	int32_t width;
	int32_t height;
	struct saved_state state;
};
/*
* Copyright (C) 2010 The Android Open Source Project
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*/

#include "pch.h"

#include "engine.h"
#include "Game.h"

#include <cstdio>

#include <android/asset_manager.h>
#include <android\asset_manager_jni.h>
#include "InputManager.h"

#include <string>
#include <sstream>

#include "SceneManager.h"
#include "Scene.h"
#include "AudioSystem.h"

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "AndroidProject1.NativeActivity", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "AndroidProject1.NativeActivity", __VA_ARGS__))

void setup_assetmanager(engine* engine)
{
	// Get path to cache dir (/data/data/org.wikibooks.OpenGL/cache)
	ANativeActivity* activity = engine->app->activity;
	JNIEnv* env = activity->env;
	JavaVM* vm = activity->vm;
	vm->AttachCurrentThread(&env, NULL);

	jclass activityClass = env->GetObjectClass(activity->clazz);

	jmethodID getCacheDir = env->GetMethodID(activityClass, "getCacheDir", "()Ljava/io/File;");
	jobject file = env->CallObjectMethod(activity->clazz, getCacheDir);
	jclass fileClass = env->FindClass("java/io/File");
	jmethodID getAbsolutePath = env->GetMethodID(fileClass, "getAbsolutePath", "()Ljava/lang/String;");
	jstring jpath = (jstring)env->CallObjectMethod(file, getAbsolutePath);
	const char* cache_dir = env->GetStringUTFChars(jpath, NULL);

	// chdir in the application cache directory
	LOGI("app_dir: %s", cache_dir);
	chdir(cache_dir);
	
	engine->cachePath = cache_dir;

	// get apk dir
	jmethodID methodID = env->GetMethodID(activityClass, "getPackageCodePath", "()Ljava/lang/String;");
	jstring result = (jstring)env->CallObjectMethod(activity->clazz, methodID);

	const char* app_path = env->GetStringUTFChars(result, NULL);
	LOGI("Looked up package code path: %s", app_path);

	engine->appPath = app_path;

	// get asset manager
	jmethodID activity_class_getAssets = env->GetMethodID(activityClass, "getAssets", "()Landroid/content/res/AssetManager;");
	jobject asset_manager = env->CallObjectMethod(activity->clazz, activity_class_getAssets);     // activity.getAssets();
	jobject mgar = env->NewGlobalRef(asset_manager);
	engine->assetManager = AAssetManager_fromJava(env, mgar);
  
	vm->DetachCurrentThread();
}

/**
* Initialize an EGL context for the current display.
*/
static int engine_init_display(struct engine* engine) {
	if (!engine->game) return 0;
	engine->game->Init(engine);
	
	if (engine->game->GetSceneManager() && engine->game->GetSceneManager()->HasScene())
	{
		AudioSystem* audioSystem = engine->game->GetSceneManager()->GetActiveScene()->GetSystem<AudioSystem>();

		if (audioSystem)
			audioSystem->Resume();
	}

	return 0;
}

/**
* Tear down the EGL context currently associated with the display.
*/
static void engine_term_display(struct engine* engine) {
	if (!engine->game) return;
	//game.ClearContext();
	
	if(engine->game->GetSceneManager() && engine->game->GetSceneManager()->HasScene())
	{
		AudioSystem* audioSystem = engine->game->GetSceneManager()->GetActiveScene()->GetSystem<AudioSystem>();

		if (audioSystem)
			audioSystem->Suspend();
	}
}

/**
* Process the next main command.
*/
static void engine_handle_cmd(struct android_app* app, int32_t cmd) {
	struct engine* engine = (struct engine*)app->userData;
	if (!engine->game) return;

	switch (cmd) {
	case APP_CMD_SAVE_STATE:
		// The system has asked us to save our current state.  Do so.
		engine->app->savedState = malloc(sizeof(struct saved_state));
		*((struct saved_state*)engine->app->savedState) = engine->state;
		engine->app->savedStateSize = sizeof(struct saved_state);
		break;
	case APP_CMD_INIT_WINDOW:
		// The window is being shown, get it ready.
		if(engine->app->window != NULL) {
			engine_init_display(engine);
		}
		break;
	case APP_CMD_TERM_WINDOW:
		// The window is being hidden or closed, clean it up.
		engine_term_display(engine);
		break;
	case APP_CMD_GAINED_FOCUS:
		// When our app gains focus
		engine->animating = 1;
		break;
	case APP_CMD_LOST_FOCUS:
		// When our app loses focus, we stop monitoring the accelerometer.
		// This is to avoid consuming battery while not being used.
		if(engine->accelerometerSensor != NULL) {
			ASensorEventQueue_disableSensor(engine->sensorEventQueue,
				engine->accelerometerSensor);
		}
		// Also stop animating.
		engine->animating = 0;
		break;
	}
}

/**
* This is the main entry point of a native application that is using
* android_native_app_glue.  It runs in its own thread, with its own
* event loop for receiving input events and doing other things.
*/
void android_main(struct android_app* state) {
	Game game;
	struct engine engine;

	memset(&engine, 0, sizeof(engine));
	engine.game = &game;

	state->userData = &engine;
	state->onAppCmd = engine_handle_cmd;
	state->onInputEvent = InputManager::OnInput;

	engine.app = state;

	if (state->savedState != NULL) {
		// We are starting with a previous saved state; restore from it.
		engine.state = *(struct saved_state*)state->savedState;
	}

	setup_assetmanager(&engine);

	engine.animating = 1;

	// loop waiting for stuff to do.
	while(!engine.game->ShouldClose()) {
		// Read all pending events.
		int ident;
		int events;
		struct android_poll_source* source;
		// If not animating, we will block forever waiting for events.
		// If animating, we loop until all events are read, then continue
		// to draw the next frame of animation.
		while(!engine.game->ShouldClose() && (ident = ALooper_pollAll(engine.animating ? 0 : -1,
			NULL,
			&events,
			(void**)&source)) >= 0
		) {
			// Process this event.
			if(source != NULL) {
				source->process(state, source);
			}
		}

		/* Run as long as there is a display */
		if (engine.display)
			engine.game->Run();
	}

	state->destroyRequested = 1;
	engine.game = nullptr;
	state->destroyed = 1;
	state->running = 0;

	exit(1);
}

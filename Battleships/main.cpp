#include "Game.h"

#ifndef ANDROID

#ifdef WINDOWS
BOOL WINAPI SetProcessDPIAware(void);
#endif

#ifdef OPENGLES
int main()
#else
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#endif
{
	Game game;
	game.SetSplashScreen("Assets/Game/Textures/UI/Loading Screens/screen.jpg");
	game.Run();

	return 0;
}

#endif
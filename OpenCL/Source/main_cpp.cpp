#include <main.h>

int CALLBACK WinMain(
	_In_  HINSTANCE hInstance,
	_In_  HINSTANCE hPrevInstance,
	_In_  LPSTR lpCmdLine,
	_In_  int nCmdShow)
{
	OpenGLGraphicsEngine graphicsEngine;
	OpenCLGameEngine gameEngine = OpenCLGameEngine(&graphicsEngine);

	while (graphicsEngine.running)
	{
		graphicsEngine.Render();
		graphicsEngine.Update();
		gameEngine.Update();
	}
	
	return 0;
}
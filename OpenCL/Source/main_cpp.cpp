#include <main.h>

int main(int argc, char* argv[])
{
	OpenGLGraphicsEngine graphicsEngine;
	OpenCLGameEngine gameEngine = OpenCLGameEngine(&graphicsEngine);

	while (graphicsEngine.running)
	{
		graphicsEngine.Render();
		graphicsEngine.Update();
	}
	
	return 0;
}
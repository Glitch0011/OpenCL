#include <main.h>

int main(int argc, char* argv[])
{
	OpenGLGraphicsEngine graphicsEngine;
	OpenCLGameEngine gameEngine;

	while (graphicsEngine.running)
	{
		graphicsEngine.Render();
		graphicsEngine.Update();
	}
	
	return 0;
}
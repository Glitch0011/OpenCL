#pragma once

//#include <SDL.h>

#define PROGRAM_NAME "Tutorial1"

#include <functional>

class GLFWwindow;

class OpenGLGraphicsEngine
{
	GLFWwindow* mainWindow;
	
public:
	unsigned int bufferID;

	bool running = true;
	const int pointCount = 520;// 1280;

	OpenGLGraphicsEngine();

	void SetupEngine();

	void Render();

	void Update();

	int GLFWCharCallback(GLFWwindow* pWindow, unsigned int pChar);

	~OpenGLGraphicsEngine();
};
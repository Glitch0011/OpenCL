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
	const int pointCount = 1024 * 5;// 1024 * 1024 * 4;

	OpenGLGraphicsEngine();

	void SetupEngine();

	void Render();

	void Update();

	int GLFWCharCallback(GLFWwindow* pWindow, unsigned int pChar);

	~OpenGLGraphicsEngine();
};
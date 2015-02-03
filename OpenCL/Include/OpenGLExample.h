#pragma once

#include <SDL.h>

#define PROGRAM_NAME "Tutorial1"

#include <functional>

class OpenGLGraphicsEngine
{
	SDL_Window* mainWindow;

public:

	SDL_GLContext mainContext;
	unsigned int bufferID;

	bool running = true;
	const int pointCount = 100;

	OpenGLGraphicsEngine();

	void SetupEngine();

	void Render();

	void Update();

	~OpenGLGraphicsEngine();
};
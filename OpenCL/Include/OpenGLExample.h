#pragma once

#include <SDL.h>

#define PROGRAM_NAME "Tutorial1"

#include <functional>

class OpenGLGraphicsEngine
{
	SDL_Window* mainWindow;
	SDL_GLContext mainContext;
	
public:
	bool running = true;

	OpenGLGraphicsEngine();

	void SetupEngine();

	void Render();

	void Update();

	~OpenGLGraphicsEngine();
};
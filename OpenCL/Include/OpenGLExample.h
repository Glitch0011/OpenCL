#include <stdio.h>
#include <stdlib.h>

/* If using gl3.h */
/* Ensure we are using opengl's core profile only */
#define GL3_PROTOTYPES 1

#include <gl/GL.h>
#include <gl/GLU.h>

#include <SDL.h>
#define PROGRAM_NAME "Tutorial1"
#include <functional>

void checkSDLError(int line = -1)
{
#ifndef NDEBUG
	const char *error = SDL_GetError();
	if (*error != '\0')
	{
		printf("SDL Error: %s\n", error);
		if (line != -1)
			printf(" + line: %i\n", line);
		SDL_ClearError();
	}
#endif
}

class OpenGLGraphicsEngine
{
	SDL_Window* mainWindow;
	SDL_GLContext mainContext;

public:
	OpenGLGraphicsEngine()
	{
		if (SDL_Init(SDL_INIT_VIDEO) != 0)
			return;

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

		if (!(mainWindow = SDL_CreateWindow(PROGRAM_NAME, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 512, 512, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN)))
			return;

		checkSDLError(__LINE__);

		mainContext = SDL_GL_CreateContext(mainWindow);
		checkSDLError(__LINE__);

		SDL_GL_SetSwapInterval(1);
	}

	void Render()
	{
		glClearColor(1.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		/* Swap our back buffer to the front */
		SDL_GL_SwapWindow(mainWindow);
		/* Wait 2 seconds */
		SDL_Delay(2000);
	}

	~OpenGLGraphicsEngine()
	{
		SDL_GL_DeleteContext(mainContext);
		SDL_DestroyWindow(mainWindow);
		SDL_Quit();
	}
};
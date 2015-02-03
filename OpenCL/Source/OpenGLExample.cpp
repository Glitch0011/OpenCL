#include <OpenGLExample.h>

#include <GL/glew.h>

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

OpenGLGraphicsEngine::OpenGLGraphicsEngine()
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

	this->SetupEngine();
}

void OpenGLGraphicsEngine::SetupEngine()
{
	static const GLfloat positions[] =
	{
		0.5f, 0.5f,
		0.2f, 0.2f,
		10.0f, -10.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		0.4f, 1.0f,
		-1.0f, 1.0f,
		0.0f, 1.0f
	};

	glewInit();

	/*GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), nullptr, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(positions), positions);*/

	glPointSize(32.0f);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	//glVertexPointer(2, GL_FLOAT, 0, nullptr);
	//glDrawArrays(GL_POINTS, 0, 8);
}

void OpenGLGraphicsEngine::Render()
{
	int Width = 500, Height = 500;

	const int gridWidth = 1024;
	const int gridHeight = 1024;
	float t1, t2;

	t1 = t2 = (float)SDL_GetTicks() * 0.001f;
	t1 = fmod(t1, 10.0f) / 10.0f;
	t2 = fmod(t2, 4.0f) / 4.0f;
	float scale = 0.5f + (-sin(t2 * 2.0 * M_PI) + 1.0f) * 1.2f;
	//glColor4f(0.4f, 0.5f, 0.9f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();

	glTranslatef((Width >> 1), (Height >> 1), 0.0f);
	glScalef(scale, scale, scale);
	glRotatef(t1 * 360.0f, 0.0f, 0.0f, 1.0f);

	glBegin(GL_POINTS);
	for (int j = 0; j<gridHeight; j += 64){
		for (int i = 0; i<gridWidth; i += 64){
			glVertex2i(i - (gridWidth >> 1), j - (gridHeight >> 1));
		}
	}
	glEnd();

	/* Swap our back buffer to the front */
	SDL_GL_SwapWindow(mainWindow);
}

void OpenGLGraphicsEngine::Update()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_ESCAPE)
				running = false;
			break;
		case SDL_QUIT:
			running = false;
			break;
		}
	}
}

OpenGLGraphicsEngine::~OpenGLGraphicsEngine()
{
	SDL_GL_DeleteContext(mainContext);
	SDL_DestroyWindow(mainWindow);
	SDL_Quit();
}
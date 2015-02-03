#include <OpenGLExample.h>

#include <GL/glew.h>
#include <vector>
#include <random>

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

	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

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

struct Point
{
	float x, y;
	//float r, g, b, a;

	Point(float x, float y, float r, float g, float b, float a)
	{
		this->x = x;
		this->y = y;
		/*this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;*/
	}
};


void OpenGLGraphicsEngine::SetupEngine()
{
	std::mt19937 random;
	std::uniform_int_distribution<uint32_t> uint_dist10(0, 50);
	std::uniform_int_distribution<uint32_t> uintd1(0, 255);

	std::vector<Point> points;
	
	for (int i = 0; i < pointCount; i++)
	{
		points.push_back(
			Point(
			uint_dist10(random),
			uint_dist10(random),
			uintd1(random),
			uintd1(random),
			uintd1(random),
			0));
	}

	glewInit();

	glGenBuffers(1, &bufferID);
	glBindBuffer(GL_ARRAY_BUFFER, bufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Point) * points.size(), nullptr, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Point) * points.size(), &points[0].x);

	GLuint vertexAttributeobject;
	glGenVertexArrays(1, &vertexAttributeobject);
	glBindVertexArray(vertexAttributeobject);
	glBindBuffer(GL_ARRAY_BUFFER, bufferID);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glBindVertexArray(vertexAttributeobject);
}

void OpenGLGraphicsEngine::Render()
{
	glViewport(0, 0, 512, 512);
	int Width = 500, Height = 500;

	const int gridWidth = 1024;
	const int gridHeight = 1024;

	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 50, 0, 50, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glPointSize(5.0);
	
	glDrawArrays(GL_POINTS, 0, pointCount);
	
	glFlush();

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
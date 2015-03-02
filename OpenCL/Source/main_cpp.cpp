#include <main.h>

#include <chrono>
#include <FrameLimiter.h>

template<typename TimeT = std::chrono::milliseconds>
struct measure
{
	template<typename F, typename ...Args> static typename TimeT::rep execution(F func, Args&&... args)
	{
		auto start = std::chrono::system_clock::now();

		func(std::forward<Args>(args)...);

		auto duration = std::chrono::duration_cast< TimeT>(std::chrono::system_clock::now() - start);

		return duration.count();
	}
};

int CALLBACK WinMain(
	_In_  HINSTANCE hInstance,
	_In_  HINSTANCE hPrevInstance,
	_In_  LPSTR lpCmdLine,
	_In_  int nCmdShow)
{
	OpenGLGraphicsEngine graphicsEngine;
	OpenCLGameEngine gameEngine = OpenCLGameEngine(&graphicsEngine);

	FrameLimiter updateLimiter(60);
	
	while (graphicsEngine.running)
	{
		auto timePassed = updateLimiter.Start();

		graphicsEngine.Render();
		graphicsEngine.Update();
		
		if (timePassed < 1)
			gameEngine.Update(timePassed);

		updateLimiter.End();
		/*auto time = measure<>::execution([&](){});*/
	}
	
	return 0;
}
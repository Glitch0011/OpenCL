#pragma once

#include <FileHelperFunctions.h>

#pragma OPENCL EXTENSION CL_APPLE_gl_sharing : enable
#pragma OPENCL EXTENSION CL_KHR_gl_sharing : enable

#include <CL/cl.hpp>
#include <vector>
#include <string>

#define ARRAY_SIZE 64

class OpenGLGraphicsEngine;

class OpenCLGameEngine
{
	cl::Context context;
	cl::Program program;
	std::vector<cl::Device> devices;
	cl::Kernel kernel;

	const int globalSize = 8;
	const int localSize = 4;
	cl_int numGroups = globalSize / localSize;

	float numberData[ARRAY_SIZE], sum[2], total, actual_sum;

	cl::Buffer inputBuffer, sumBuffer;
	
	OpenGLGraphicsEngine* glEngine;

public:
	OpenCLGameEngine(OpenGLGraphicsEngine* glEngine);

	void SetupData();

	void Update();

	~OpenCLGameEngine();
};
#pragma once

#include <FileHelperFunctions.h>

#pragma OPENCL EXTENSION CL_APPLE_gl_sharing : enable
#pragma OPENCL EXTENSION CL_KHR_gl_sharing : enable

#include <CL/cl.hpp>
#include <vector>
#include <string>
#include <map>
#include <real_t.h>

class OpenGLGraphicsEngine;

class OpenCLGameEngine
{
	cl::Context context;
	cl::Program program;
	cl::Kernel kernel;

	std::map<std::string, cl::Kernel> kernels;
	std::vector<cl::Device> devices;
	cl::Device device;

	cl::BufferGL graphicsBuffer;
	cl::Buffer timeBuffer;

	cl_int err = CL_SUCCESS;

	cl::Event event;
	cl::CommandQueue queue;

	int globalSize = 8;
	int localSize = 4;
	cl_int numGroups = globalSize / localSize;

	cl::Buffer inputBuffer, sumBuffer;
	
	OpenGLGraphicsEngine* glEngine;

public:
	OpenCLGameEngine(OpenGLGraphicsEngine* glEngine);

	void SetupData();

	void Update(real_t);

	~OpenCLGameEngine();
};
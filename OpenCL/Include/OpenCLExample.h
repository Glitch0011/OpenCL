#pragma once

#include <FileHelperFunctions.h>

#pragma OPENCL EXTENSION CL_APPLE_gl_sharing : enable
#pragma OPENCL EXTENSION CL_KHR_gl_sharing : enable

#include <CL/cl.hpp>
#include <vector>
#include <string>
#include <map>

#define ARRAY_SIZE 64

class OpenGLGraphicsEngine;

class OpenCLGameEngine
{
	cl::Context context;
	cl::Program program;
	std::vector<cl::Device> devices;
	cl::Kernel kernel;
	std::map<std::string, cl::Kernel> kernels;
	
	cl::BufferGL graphicsBuffer;
	cl::Buffer timeBuffer;

	cl::Device device;

	cl_int err = CL_SUCCESS;

	cl::Event event;
	cl::CommandQueue queue;

	int globalSize = 8;
	int localSize = 4;
	cl_int numGroups = globalSize / localSize;

	//float numberData[100 * 2];// sum[2], total, actual_sum;

	cl::Buffer inputBuffer, sumBuffer;
	
	OpenGLGraphicsEngine* glEngine;

public:
	OpenCLGameEngine(OpenGLGraphicsEngine* glEngine);

	void SetupData();

	void Update(double);

	~OpenCLGameEngine();
};
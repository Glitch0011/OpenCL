#pragma once

#include <FileHelperFunctions.h>
#include <CL/cl.hpp>
#include <vector>
#include <string>


#define ARRAY_SIZE 64

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
	
public:
	OpenCLGameEngine();

	void SetupData();

	void Update();

	~OpenCLGameEngine();
};
#include <iostream>
#include <CL/cl.h>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

static std::vector<char> loadText(std::string file)
{
	ifstream ifs(file.c_str(), ios::binary | ios::ate);
	ifstream::pos_type pos = ifs.tellg();

	std::vector<char> result(pos);

	ifs.seekg(0, ios::beg);
	ifs.read(&result[0], pos);

	return result;
}

int setupDevice(cl_platform_id& platform, cl_device_id* device)
{
	cl_int err = 0;
	if ((err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 1, device, nullptr)) != CL_SUCCESS)
	{
		switch (err)
		{
		case CL_INVALID_PLATFORM:
			return -1;
		case CL_INVALID_DEVICE_TYPE:
			return -1;
		case CL_INVALID_VALUE:
			return -1;
		case CL_DEVICE_NOT_FOUND:
			return -1;
		default:
			return -1;
		}
		return -1;
	}

	return 0;
}

int setupContext(cl_device_id* device, cl_context* context)
{
	cl_int err = 0;
	*context = clCreateContext(nullptr, 1, device, nullptr, nullptr, &err);
	if (err != CL_SUCCESS)
		return -1;

	return 0;
}

int setupProgram(cl_context& context, std::string fileName, cl_program* program)
{
	cl_int err;

	auto data = loadText(fileName);
	char* dataData = data.data();
	const size_t dataSize = data.size();

	*program = clCreateProgramWithSource(context, 1, (const char**)&dataData, &dataSize, &err);

	if ((err = clBuildProgram(*program, 0, nullptr, nullptr, nullptr, nullptr)) != CL_SUCCESS)
	{
		switch (err)
		{
		case CL_INVALID_PROGRAM:
			return -1;
		case CL_INVALID_VALUE:
			return -1;
		case CL_INVALID_DEVICE:
			return -1;
		case CL_INVALID_BINARY:
			return -1;
		case CL_INVALID_BUILD_OPTIONS:
			return -1;
		case CL_INVALID_OPERATION:
			return -1;
		case CL_COMPILER_NOT_AVAILABLE:
			return -1;
		case CL_BUILD_PROGRAM_FAILURE:
			return -1;
		case CL_OUT_OF_HOST_MEMORY:
			return -1;
		default:
			return -1;
		}
		return -1;
	}

	return 0;
}

int setupOpenCL(cl_device_id* device, cl_context* context, cl_program* program)
{
	cl_int err;

	cl_platform_id platform;
	if ((err = clGetPlatformIDs(1, &platform, nullptr)) != CL_SUCCESS)
		return -1;

	if (setupDevice(platform, device) != CL_SUCCESS)
		return -1;

	if (setupContext(device, context) != CL_SUCCESS)
		return -1;

	setupProgram(*context, ".\\..\\Compute\\SimpleAddition.cl", program);	
}

int setupKernel(cl_program& program, std::string functionName, cl_kernel* kernel)
{
	cl_int err;
	*kernel = clCreateKernel(program, functionName.c_str(), &err);

	if (err != CL_SUCCESS)
	{
		switch (err)
		{
		case CL_INVALID_PROGRAM:
			return -1;
		case CL_INVALID_PROGRAM_EXECUTABLE:
			return -1;
		case CL_INVALID_KERNEL_NAME:
			return -1;
		case CL_INVALID_KERNEL_DEFINITION:
			return -1;
		case CL_INVALID_VALUE:
			return -1;
		case CL_OUT_OF_HOST_MEMORY:
			return -1;
		}
	}
}

int main()
{
	cl_device_id device;
	cl_context context;
	cl_program program;

	setupOpenCL(&device, &context, &program);

	const int ARRAY_SIZE = 64;
	const size_t globalSize = 8, localSize = 4;

	float data[ARRAY_SIZE], sum[2], total, actual_sum;

	cl_mem inputBuffer, sumBuffer;
	cl_int numGroups = globalSize / localSize;;

	for (int i = 0; i < ARRAY_SIZE; i++)
		data[i] = 1.0f * i;

	cl_int err;

	inputBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, ARRAY_SIZE * sizeof(float), data, &err);
	sumBuffer = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, numGroups * sizeof(float), sum, &err);

	if (err != CL_SUCCESS)
		return -1;
	
	cl_command_queue queue = clCreateCommandQueue(context, device, 0, &err);
	if (err != CL_SUCCESS)
		return -1;

	cl_kernel kernel;
	setupKernel(program, "add_numbers", &kernel);

	err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &inputBuffer);
	err |= clSetKernelArg(kernel, 1, localSize * sizeof(float), nullptr);
	err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &sumBuffer);
	if (err < 0)
		return -1;

	clEnqueueNDRangeKernel(queue, kernel, 1, nullptr, &globalSize, &localSize, 0, nullptr, nullptr);

	err = clEnqueueReadBuffer(queue, sumBuffer, CL_TRUE, 0, sizeof(sum), sum, 0, nullptr, nullptr);
	if (err != CL_SUCCESS)
		return -1;

	total = 0.0f;
	for (int i = 0; i < numGroups; i++)
		total += sum[i];

	actual_sum = 1.0f * ARRAY_SIZE / 2 * (ARRAY_SIZE - 1);

	printf("Computed = %.1f.\n", total);

	if (fabs(total - actual_sum) > 0.01*fabs(actual_sum))
		printf("Check failed.\n");
	else
		printf("Check passed.\n");

	/* Deallocate resources */
	clReleaseKernel(kernel);
	clReleaseMemObject(sumBuffer);
	clReleaseMemObject(inputBuffer);
	clReleaseCommandQueue(queue);
	clReleaseProgram(program);
	clReleaseContext(context);
	return 0;
}
#include <OpenCLExample.h>
#include <OpenGLExample.h>
//#include <test.h>

typedef cl_int _stdcall _clGetGLContextInfoKHR(
	const cl_context_properties*, 
	cl_gl_context_info, 
	size_t, 
	void*,
	size_t*);

OpenCLGameEngine::OpenCLGameEngine(OpenGLGraphicsEngine* glEngine)
{
	cl_int err = CL_SUCCESS;

	try
	{
		/*std::vector<cl::Platform> platforms;
		cl::Platform::get(&platforms);

		if (platforms.size() == 0)
			return;

		cl_context_properties properties[] =
		{
			CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[1])(),
			//CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(),
			//CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(),
			0
		};

		context = cl::Context(CL_DEVICE_TYPE_GPU, properties);

		devices = context.getInfo<CL_CONTEXT_DEVICES>();*/

		/*size_t bytes = 0;
		auto i = clGetExtensionFunctionAddress("clGetGLContextInfoKHR");
		auto res = ((_clGetGLContextInfoKHR*)i)(properties, CL_DEVICES_FOR_GL_CONTEXT_KHR, 0, nullptr, &bytes);
		size_t devNum = bytes / sizeof(cl_device_id);
		std::vector<cl_device_id> devs(devNum);
		res = ((_clGetGLContextInfoKHR*)i)(properties, CL_DEVICES_FOR_GL_CONTEXT_KHR, bytes, &devs, nullptr);*/

		cl_uint lNbPlatformId = 0;
		cl_int err = 0;

		clGetPlatformIDs(0, 0, &lNbPlatformId);

		if (lNbPlatformId == 0)
		{
			return;
		}


		// Loop on all platforms.
		std::vector< cl_platform_id > lPlatformIds(lNbPlatformId);
		clGetPlatformIDs(lNbPlatformId, lPlatformIds.data(), 0);

		// Try to find the device with the compatible context.
		cl_platform_id lPlatformId = 0;
		cl_device_id lDeviceId = 0;
		cl_context lContext = 0;

		for (size_t i = 0; i < lPlatformIds.size() && lContext == 0; ++i)
		{
			const cl_platform_id lPlatformIdToTry = lPlatformIds[i];

			// Get devices.
			cl_uint lNbDeviceId = 0;
			clGetDeviceIDs(lPlatformIdToTry, CL_DEVICE_TYPE_GPU, 0, 0, &lNbDeviceId);

			if (lNbDeviceId == 0)
			{
				continue;
			}

			std::vector< cl_device_id > lDeviceIds(lNbDeviceId);
			clGetDeviceIDs(lPlatformIdToTry, CL_DEVICE_TYPE_GPU, lNbDeviceId, lDeviceIds.data(), 0);


			// Create the properties for this context.
			cl_context_properties lContextProperties[] = {
				// We need to add information about the OpenGL context with
				// which we want to exchange information with the OpenCL context.
				// We should first check for cl_khr_gl_sharing extension.
				CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(),
				CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(),
				CL_CONTEXT_PLATFORM, (cl_context_properties)lPlatformIdToTry,
				0, 0,
			};


			// Look for the compatible context.
			for (size_t j = 0; j < lDeviceIds.size(); ++j)
			{
				cl_device_id lDeviceIdToTry = lDeviceIds[j];
				cl_context lContextToTry = 0;

				lContextToTry = clCreateContext(
					lContextProperties,
					1, &lDeviceIdToTry,
					0, 0,
					&err
					);
				if (err == CL_SUCCESS)
				{
					// We found the context.
					lPlatformId = lPlatformIdToTry;
					lDeviceId = lDeviceIdToTry;
					lContext = lContextToTry;
					break;
				}
			}
		}

		this->context() = lContext;
		//this->deviceID = lDeviceId;
		//this->platform = lPlatformId;

		std::vector<char> data = loadText(".\\..\\Compute\\SimpleAddition.cl");

		cl::Program::Sources source(1, std::make_pair((char*)data.data(), data.size()));
		program = cl::Program(context, source);
		std::vector<cl::Device> realDevices{ lDeviceId };
		program.build(realDevices);

		this->glEngine = glEngine;

		SetupData();
	}
	catch (exception e)
	{
		return;
	}
}

void OpenCLGameEngine::SetupData()
{
	cl_int err = CL_SUCCESS;

	this->graphicsBuffer = cl::BufferGL(this->context, CL_MEM_READ_WRITE, this->glEngine->bufferID, &err);

	/*for (int i = 0; i < ARRAY_SIZE; i++)
		numberData[i] = 1.0f * i;

	inputBuffer = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, ARRAY_SIZE * sizeof(float), numberData, &err);
	sumBuffer = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, numGroups * sizeof(float), sum, &err);*/

	kernel = cl::Kernel(program, "add_numbers", &err);

	kernel.setArg(0, graphicsBuffer);
	//kernel.setArg(1, localSize * sizeof(float), nullptr);
	//kernel.setArg(2, sumBuffer);
}

void OpenCLGameEngine::Update()
{
	cl_int err = CL_SUCCESS;

	cl::Event event;
	cl::CommandQueue queue(context, devices[0], 0, &err);
	queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(globalSize), cl::NDRange(localSize), nullptr, &event);

	event.wait();

	queue.enqueueReadBuffer(sumBuffer, true, 0, sizeof(sum), sum);

	total = 0.0f;
	for (int i = 0; i < numGroups; i++)
		total += sum[i];

	actual_sum = 1.0f * ARRAY_SIZE / 2 * (ARRAY_SIZE - 1);

	printf("Computed = %.1f.\n", total);

	if (fabs(total - actual_sum) > 0.01 * fabs(actual_sum))
		printf("Check failed.\n");
	else
		printf("Check passed.\n");
}

OpenCLGameEngine::~OpenCLGameEngine()
{

}
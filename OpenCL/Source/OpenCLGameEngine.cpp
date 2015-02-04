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
		this->device() = lDeviceId;
		//this->platform = lPlatformId;

		std::vector<std::vector<char>> data =
		{
			loadText(".\\..\\Compute\\Header.cl"),
			loadText(".\\..\\Compute\\SPH_Header.cl"),
			loadText(".\\..\\Compute\\SPH_Functions.cl"),
			loadText(".\\..\\Compute\\SimpleAddition.cl"),
			loadText(".\\..\\Compute\\SPH_DensityAndPressure.cl"),
			loadText(".\\..\\Compute\\SPH_Viscosity.cl"),
			loadText(".\\..\\Compute\\SPH_UpdateBoids.cl"),
		};

		std::vector<std::pair<const char*, size_t>> sources;
		for (auto& dat : data)
			sources.push_back(std::make_pair((char*)dat.data(), dat.size()));

		cl::Program::Sources source(sources);

		program = cl::Program(context, source);
		std::vector<cl::Device> realDevices{ lDeviceId };
		err = program.build(realDevices);
		if (err != 0)
		{
			size_t length;
			std::string str;
			program.getBuildInfo<std::string>(this->device, CL_PROGRAM_BUILD_LOG, &str);
			return;
		}

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

	//kernel = cl::Kernel(program, "add_numbers", &err);
	kernels["1"] = cl::Kernel(program, "calculateDensityAndPressure", &err);
	kernels["2"] = cl::Kernel(program, "calculateViscosity", &err);
	kernels["3"] = cl::Kernel(program, "calculateUpdateBoids", &err);

	kernel.setArg(0, graphicsBuffer);

	queue = cl::CommandQueue(this->context, this->device, 0, &err);

	/*size_t retSize = 0;
	kernel.getWorkGroupInfo(this->device, CL_KERNEL_WORK_GROUP_SIZE, &retSize);*/
	
	this->globalSize = this->glEngine->pointCount;
	this->localSize = 1;

	numGroups = globalSize / localSize;
}

void OpenCLGameEngine::Update()
{
	cl_uint err = 0;
	cl::Event lastWait;
	std::vector<cl::Memory> objs = { this->graphicsBuffer };
	err = queue.enqueueAcquireGLObjects(&objs, nullptr, &lastWait);

	for (auto& k : kernels)
	{
		k.second.setArg(0, graphicsBuffer);

		vector<cl::Event> secondWaits = { lastWait };
		err = queue.enqueueNDRangeKernel(k.second, cl::NullRange, cl::NDRange(globalSize), cl::NDRange(localSize), &secondWaits, &lastWait);
	}

	vector<cl::Event> thirdWaits = { lastWait };
	err = queue.enqueueReleaseGLObjects(&objs, &thirdWaits, &lastWait);

	lastWait.wait();
}

OpenCLGameEngine::~OpenCLGameEngine()
{

}
#include <OpenCLExample.h>
#include <OpenGLExample.h>
//#include <test.h>

typedef cl_int _stdcall _clGetGLContextInfoKHR(
	const cl_context_properties*, 
	cl_gl_context_info, 
	size_t, 
	void*,
	size_t*);

#include <Point.h>

//http://stackoverflow.com/questions/1149620/how-to-write-to-the-output-window-in-visual-studios
void Output(const char* szFormat, ...)
{
	char szBuff[1024];
	va_list arg;
	va_start(arg, szFormat);
	_vsnprintf_s(szBuff, sizeof(szBuff), szFormat, arg);
	va_end(arg);

	OutputDebugString(szBuff);
}

OpenCLGameEngine::OpenCLGameEngine(OpenGLGraphicsEngine* glEngine)
{
	cl_int err = CL_SUCCESS;

	this->glEngine = glEngine;

	try
	{
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

				cl_int errRet;

				lContextToTry = clCreateContext(
					lContextProperties,
					1, &lDeviceIdToTry,
					[](const char* a, const void* b, size_t c, void* d)
					{
						return;
					} , nullptr,
					&errRet);

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

		std::vector<std::vector<char>> data =
		{
			loadText(".\\..\\..\\Compute\\Header.cl"),
			loadText(".\\..\\..\\Compute\\SPH_Header.cl"),
			loadText(".\\..\\..\\Compute\\SPH_Functions.cl"),
			loadText(".\\..\\..\\Compute\\SPH_DensityAndPressure.cl"),
			loadText(".\\..\\..\\Compute\\SPH_Viscosity.cl"),
			loadText(".\\..\\..\\Compute\\SPH_UpdateBoids.cl"),
		};
		
		std::vector<std::pair<const char*, size_t>> sources;
		for (auto& dat : data)
			sources.push_back(std::make_pair((char*)dat.data(), dat.size()));

		/*char test[1024];
		size_t s = 1024;
		clGetDeviceInfo(this->device(), CL_DEVICE_EXTENSIONS, s, test, &s);*/
		
		cl::Program::Sources source(sources);

		program = cl::Program(context, source);
		std::vector<cl::Device> realDevices{ lDeviceId };
		auto opts = "-cl-finite-math-only -DboidCount=" + std::to_string(this->glEngine->pointCount);

#ifdef useDouble
		opts += " -DCONFIG_USE_DOUBLE";
#endif

		err = program.build(realDevices, opts.c_str());
		if (err != 0)
		{
			size_t length;
			std::string str;
			program.getBuildInfo<std::string>(this->device, CL_PROGRAM_BUILD_LOG, &str);
			Output("%s", str);
			return;
		}

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

	real_t time = 0.0f;
	this->timeBuffer = cl::Buffer(this->context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(real_t), &time, &err);

	kernels["1"] = cl::Kernel(program, "calculateDensityAndPressure", &err);
	kernels["2"] = cl::Kernel(program, "calculateViscosity", &err);
	kernels["3"] = cl::Kernel(program, "calculateUpdateBoids", &err);

	queue = cl::CommandQueue(this->context, this->device, 0, &err);

	/*size_t retSize = 0;
	kernels["1"].getWorkGroupInfo(this->device, CL_KERNEL_WORK_GROUP_SIZE, &retSize);*/
	
	this->globalSize = this->glEngine->pointCount;
	this->localSize = 1;

	numGroups = globalSize / localSize;
}

#include <Point.h>

void OpenCLGameEngine::Update(real_t timePassedInSeconds)
{
	cl_uint err = 0;
	cl::Event lastWait;
	vector<cl::Event> waits;

	timePassedInSeconds *= 0.25;

	real_t f = timePassedInSeconds;
	err = queue.enqueueWriteBuffer(this->timeBuffer, false, 0, sizeof(real_t), &f, nullptr, &lastWait);

	std::vector<cl::Memory> objs = { this->graphicsBuffer };
	waits = { lastWait };
	err = queue.enqueueAcquireGLObjects(&objs, &waits, &lastWait);

	for (auto& k : kernels)
	{
		k.second.setArg(0, this->graphicsBuffer);
		k.second.setArg(1, this->timeBuffer); //Only the 3rd Kernel really needs this
		
		waits = { lastWait };
		err = queue.enqueueNDRangeKernel(k.second, cl::NullRange, cl::NDRange(globalSize), cl::NDRange(localSize), &waits, &lastWait);
	}

	waits = { lastWait };
	err = queue.enqueueReleaseGLObjects(&objs, &waits, &lastWait);

	lastWait.wait();

	//Reads the data
	/*void* data = queue.enqueueMapBuffer(this->graphicsBuffer, true, CL_MAP_READ, 0, sizeof(Point) * this->glEngine->pointCount);
	queue.enqueueUnmapMemObject(this->graphicsBuffer, data, nullptr, &lastWait);*/
}

OpenCLGameEngine::~OpenCLGameEngine()
{

}
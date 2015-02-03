#include <OpenCLExample.h>

using namespace cl;

OpenCLGameEngine::OpenCLGameEngine()
{
	cl_int err = CL_SUCCESS;

	try
	{
		std::vector<Platform> platforms;
		Platform::get(&platforms);

		if (platforms.size() == 0)
			return;

		cl_context_properties properties[] =
		{
			CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[0])(), 0
		};

		context = Context(CL_DEVICE_TYPE_CPU, properties);

		devices = context.getInfo<CL_CONTEXT_DEVICES>();

		std::vector<char> data = loadText(".\\..\\Compute\\SimpleAddition.cl");

		Program::Sources source(1, std::make_pair((char*)data.data(), data.size()));
		program = Program(context, source);
		program.build(devices);

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


	for (int i = 0; i < ARRAY_SIZE; i++)
		numberData[i] = 1.0f * i;

	inputBuffer = Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, ARRAY_SIZE * sizeof(float), numberData, &err);
	sumBuffer = Buffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, numGroups * sizeof(float), sum, &err);

	kernel = Kernel(program, "add_numbers", &err);

	kernel.setArg(0, inputBuffer);
	kernel.setArg(1, localSize * sizeof(float), nullptr);
	kernel.setArg(2, sumBuffer);
}

void OpenCLGameEngine::Update()
{
	cl_int err = CL_SUCCESS;

	Event event;
	CommandQueue queue(context, devices[0], 0, &err);
	queue.enqueueNDRangeKernel(kernel, NullRange, NDRange(globalSize), NDRange(localSize), nullptr, &event);

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
#include <FileHelperFunctions.h>
#include <CL/cl.hpp>
#include <vector>
#include <string>

using namespace cl;
using namespace std;

int main()
{
	cl_int err = CL_SUCCESS;

	try
	{
		std::vector<Platform> platforms;
		Platform::get(&platforms);

		if (platforms.size() == 0)
			return -1;

		cl_context_properties properties[] =
		{
			CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[0])(), 0
		};
		Context context(CL_DEVICE_TYPE_CPU, properties);

		std::vector<Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();

		std::vector<char> data = loadText(".\\..\\Compute\\SimpleAddition.cl");

		Program::Sources source(1, std::make_pair((char*)data.data(), data.size()));
		Program program = Program(context, source);
		program.build(devices);

		const int ARRAY_SIZE = 64;
		const int globalSize = 8;
		const int localSize = 4;

		float numberData[ARRAY_SIZE], sum[2], total, actual_sum;

		cl_int numGroups = globalSize / localSize;;

		for (int i = 0; i < ARRAY_SIZE; i++)
			numberData[i] = 1.0f * i;

		cl::Buffer inputBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, ARRAY_SIZE * sizeof(float), numberData, &err);
		cl::Buffer sumBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, numGroups * sizeof(float), sum, &err);

		Kernel kernel(program, "add_numbers", &err);

		kernel.setArg(0, inputBuffer);
		kernel.setArg(1, localSize * sizeof(float), nullptr);
		kernel.setArg(2, sumBuffer);

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
	catch (exception e)
	{
		return -1;
	}
	return 0;
}
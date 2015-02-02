
__kernel void add_numbers(__global float4* data, __local float* local_result, __global float* group_result)
{
	uint global_addr = get_global_id(0) * 2;
	
	float4 input1 = data[global_addr + 0];
	float4 input2 = data[global_addr + 1];

	float4 sum_vector = input1 + input2;

	uint local_addr = get_local_id(0);
	local_result[local_addr] = sum_vector.s0 + sum_vector.s1 + sum_vector.s2 + sum_vector.s3; 

	barrier(CLK_LOCAL_MEM_FENCE);

	if (get_local_id(0) == 0)
	{
		float sum = 0.0f;

		for(int i = 0; i < get_local_size(0); i++)
			sum += local_result[i];

		group_result[get_group_id(0)] = sum;
	}
}
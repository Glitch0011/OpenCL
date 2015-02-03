
typedef struct Boid{
  float2 pos;
  float2 vel;
}Boid;

__kernel void add_numbers(__global Boid* data)//, __local float* local_result, __global float* group_result)
{
	uint global_addr = get_global_id(0);
	
	Boid boid = data[global_addr];

	float2 gravity; gravity.y = -0.01f; gravity.x = 0;

	//float2 dir = normalize(gravity - boid.pos) * 0.1f;

	boid.vel += gravity;
	boid.vel *= 0.99f;

	float2 futurePos = boid.pos + boid.vel;
	if (futurePos.y < 0)
		boid.vel *= -1;
	else
		boid.pos = futurePos;

	data[global_addr] = boid;

	/*uint local_addr = get_local_id(0);
	local_result[local_addr] = sum_vector.s0 + sum_vector.s1 + sum_vector.s2 + sum_vector.s3; 

	barrier(CLK_LOCAL_MEM_FENCE);

	if (get_local_id(0) == 0)
	{
		float sum = 0.0f;

		for(int i = 0; i < get_local_size(0); i++)
			sum += local_result[i];

		group_result[get_group_id(0)] = sum;
	}*/
}

__kernel void calculateDensityAndPressure(__global Boid* data)
{
	uint global_addr = get_global_id(0);

	Boid boid = data[global_addr];
	boid.density = 0;

	for (int i = 0; i < boidCount; ++i)
	{
		real_t radSqr = lengthSquared((boid.pos - data[i].pos).xyz);
		if (radSqr <= h_constant * h_constant)
			boid.density += Wpoly6(radSqr);
	}

	boid.density *= boid.mass;

	boid.pressure = GAS_STIFFNESS * (boid.density - REST_DENSITY);

	//barrier(CLK_GLOBAL_MEM_FENCE);

	data[global_addr] = boid;
}
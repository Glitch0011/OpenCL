
__kernel void calculateUpdateBoids(__global Boid* data, __constant real_t* _timePassedInSeconds)
{
	real_t timePassedInSeconds = *_timePassedInSeconds;

	uint global_addr = get_global_id(0);
	Boid boid = data[global_addr];

	/*real_t l = length(boid.velocity);
	real_t speedLimit = 0.1;
	if (l > speedLimit)
		boid.velocity = normalize(boid.velocity) * speedLimit;*/

	real4_t newPos = boid.pos + (boid.velocity * timePassedInSeconds) + (boid.accel * timePassedInSeconds * timePassedInSeconds);

	real4_t newVel = (newPos - boid.pos) / timePassedInSeconds;

	boid.pos = newPos;
	boid.velocity = newVel;

	//barrier(CLK_GLOBAL_MEM_FENCE);

	data[global_addr] = boid;
}
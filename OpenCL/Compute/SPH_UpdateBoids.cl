
#define timePassedInSeconds 0.000001f

__kernel void calculateUpdateBoids(__global Boid* data)
{
	uint global_addr = get_global_id(0);
	Boid boid = data[global_addr];

	real4_t newPos = boid.pos + (boid.velocity * timePassedInSeconds) + (boid.accel * timePassedInSeconds * timePassedInSeconds);

	real4_t newVel = (newPos - boid.pos) / timePassedInSeconds;

	/*if (!intersect(quadTree->boundary, newPos) || !(newPos[0] < 0 || newPos[0] > 0 || newPos[0] == 0))
	{
		newPos = Vec3d(uniform_distribution(generator), uniform_distribution(generator), 1);
		newVel = Vec3d(0, 0, 0);
	}*/

	boid.pos = newPos;
	boid.velocity = newVel;
	
	data[global_addr] = boid;
}
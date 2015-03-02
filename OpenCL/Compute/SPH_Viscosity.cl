typedef struct WALL
{
	real3_t point;
	real3_t normal;
} WALL;

void collisionForce(Boid* boid)
{
	WALL _walls[4];
	_walls[0].normal.x = 1;
	_walls[0].normal.y = 0;
	_walls[0].normal.z = 0;
	_walls[0].point.x = -boundarySize;
	_walls[0].point.y = 0;
	_walls[0].point.z = 0;

	_walls[1].normal.x = -1;
	_walls[1].normal.y = 0;
	_walls[1].normal.z = 0;
	_walls[1].point.x = boundarySize;
	_walls[1].point.y = 0;
	_walls[1].point.z = 0;

	_walls[2].normal.x = 0;
	_walls[2].normal.y = -1;
	_walls[2].normal.z = 0;
	_walls[2].point.x = 0;
	_walls[2].point.y = boundarySize;
	_walls[2].point.z = 0;

	_walls[3].normal.x = 0;
	_walls[3].normal.y = 1;
	_walls[3].normal.z = 0;
	_walls[3].point.x = 0;
	_walls[3].point.y = -boundarySize;
	_walls[3].point.z = 0;

	for (int i = 0; i < 4; ++i)
	{
		WALL wall = _walls[i];

		real_t d = dot(wall.point - boid->pos.xyz, wall.normal) + 0.01; // particle radius

		if (d > 0.0)
		{
			boid->accel.xyz += WALL_K * wall.normal * d;
			boid->accel.xyz -= WALL_DAMPING * dot(boid->velocity.xyz, wall.normal) * wall.normal;
		}
	}
}

__kernel void calculateViscosity(__global Boid* data)
{
	uint global_addr = get_global_id(0);
	Boid boid = data[global_addr];

	real3_t gravity = (real3_t)(0.0f, -GRAVITY_ACCELERATION, 0.0f);

	real3_t f_pressure, f_viscosity, f_surface, f_gravity, colorFieldNormal;
	f_pressure.xyz = 0;
	f_viscosity.xyz = 0;
	f_surface.xyz = 0;
	f_gravity = (gravity * boid.density);
	colorFieldNormal.xyz = 0;

	real_t colorFieldLaplacian = 0;

	for (int i = 0; i < boidCount; ++i)
	{
		Boid otherBoid = data[i];

		real3_t diffPosition = (boid.pos - otherBoid.pos).xyz;
		real_t radiusSquared = lengthSquared(diffPosition);

		if (radiusSquared <= h_constant * h_constant)
		{
			real3_t poly6Gradient, spikyGradient;

			poly6Gradient = Wpoly6Gradient(diffPosition, radiusSquared);

			spikyGradient = WspikyGradient(diffPosition, radiusSquared);

			if (global_addr != i) //boid != otherBoid)
			{
				f_pressure += (boid.pressure / pow(boid.density, 2) + otherBoid.pressure / pow(otherBoid.density, 2)) * spikyGradient;
				f_viscosity += (otherBoid.velocity - boid.velocity).xyz * WviscosityLaplacian(radiusSquared) / otherBoid.density;
			}

			colorFieldNormal += poly6Gradient / otherBoid.density;

			colorFieldLaplacian += Wpoly6Laplacian(radiusSquared) / otherBoid.density;
		}
	}

	f_pressure *= -boid.mass * boid.density;

	f_viscosity *= VISCOSITY * boid.mass;

	colorFieldNormal *= boid.mass;

	colorFieldLaplacian *= boid.mass;

	//Surface tension
	real_t colorFieldNormalMagnitude = length(colorFieldNormal);
		
	if (colorFieldNormalMagnitude > SURFACE_THRESHOLD)
	{
		f_surface = -SURFACE_TENSION * colorFieldNormal / colorFieldNormalMagnitude * colorFieldLaplacian;
	}

	boid.accel.xyz = (f_pressure + f_viscosity + f_surface + f_gravity) / boid.density;
	boid.accel.zw = 0;

	collisionForce(&boid);

	//barrier(CLK_GLOBAL_MEM_FENCE);

	data[global_addr] = boid;
}
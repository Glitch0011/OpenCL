#pragma once

#include <CL/cl.h>
#include <real_t.h>

struct Pos
{
	real_t x, y, z, w;

	Pos()
	{
		this->x = 0;
		this->y = 0;
		this->z = 0;
		this->w = 0;
	}
};

struct Point
{
	Pos pos;
	Pos accel;
	Pos velocity;
	real_t pressure;
	real_t density;
	real_t mass;
	real_t temp;

	Point(real_t x, real_t y)
	{
		this->pos.x = x;
		this->pos.y = y;

		this->pressure = 0;
		this->density = 0;
		this->mass = 0.02;
		this->temp = 0.0;
	}
};
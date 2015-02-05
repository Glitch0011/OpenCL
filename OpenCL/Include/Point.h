#pragma once

#include <CL/cl.h>

//#define useDouble

#ifdef useDouble
	#define real_t double
	#define gl_real_t GL_DOUBLE
#else
	#define real_t float
	#define gl_real_t GL_FLOAT
#endif

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
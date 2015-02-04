//#define CONFIG_USE_DOUBLE

#if defined(CONFIG_USE_DOUBLE)

	#if defined(cl_khr_fp64)  // Khronos extension available?
		#pragma OPENCL EXTENSION cl_khr_fp64 : enable
		#define DOUBLE_SUPPORT_AVAILABLE
	#elif defined(cl_amd_fp64)  // AMD extension available?
		#pragma OPENCL EXTENSION cl_amd_fp64 : enable
		#define DOUBLE_SUPPORT_AVAILABLE
	#endif

#endif

#if defined(DOUBLE_SUPPORT_AVAILABLE)

	// double
	typedef double real_t;
	typedef double2 real2_t;
	typedef double3 real3_t;
	typedef double4 real4_t;
	typedef double8 real8_t;
	typedef double16 real16_t;
	#define PI 3.14159265358979323846

#else

	// float
	typedef float real_t;
	typedef float2 real2_t;
	typedef float3 real3_t;
	typedef float4 real4_t;
	typedef float8 real8_t;
	typedef float16 real16_t;
	#define PI 3.14159265359f

#endif

typedef struct Boid{
  real4_t pos;
  real4_t accel;
  real4_t velocity;
  real_t pressure;
  real_t density;
  real_t mass;
  real_t temp;
}Boid;

/*typedef struct Boid{
  real3_t pos;
  real3_t accel;
  real3_t velocity;
  real_t pressure;
  real_t density;
  real_t mass;
}Boid;*/
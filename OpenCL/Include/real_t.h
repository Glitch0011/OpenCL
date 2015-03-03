#ifdef useDouble
	#define real_t double
	#define gl_real_t GL_DOUBLE
#else
	#define real_t float
	#define gl_real_t GL_FLOAT
#endif
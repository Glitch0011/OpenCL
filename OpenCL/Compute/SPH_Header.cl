
#define h_constant 0.035f
/*0.0457;*/

//#define boidCount (128)
//

#define GAS_STIFFNESS 3.0
 /*3.0 //20.0 // 461.5  // Nm/kg is gas constant of water vapor*/

//Lower = less compression
#define REST_DENSITY 998.29
/*998.29 // kg/m^3 is rest density of water particle*/

#define PARTICLE_MASS 0.02f
/* kg */

//Higher = gloopier
#define VISCOSITY 3.5f
 /*150// 3.5 // 5.0 // 0.00089 // Ns/m^2 or Pa*s viscosity of water*/

#define SURFACE_TENSION 0.0728f
 /* N/m*/

#define SURFACE_THRESHOLD 0.01f
 /*7.065*/

#define KERNEL_PARTICLES 20.0f

#define GRAVITY_ACCELERATION 9.80665f

#define WALL_K 10000.0 
/*10000.0 // wall spring constant*/

#define WALL_DAMPING 30
 /*-0.9 // wall damping constant*/

#define BOX_SIZE 0.4
#define PADDING 0.1
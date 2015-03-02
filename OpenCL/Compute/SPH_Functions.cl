#define pow2(x) (x*x)
#define pow3(x) (x*x*x)
#define pow4(x) (x*x*x*x)
#define pow6(x) (x*x*x*x*x*x)
#define pow9(x) (x*x*x*x*x*x*x*x*x)

real_t W(real3_t ij)
{
	real_t q = length(ij) / h_constant;

	if (q >= 0 || q <= 2)
		q = (2.0 / 3.0) - (9.0 / 8.0) * pow(q, 2) + (19.0 / 24.0) * pow(q, 3) - (5.0 / 32.0) * pow(q, 4);
	else
		q = 0;

	return q;
}

__constant real_t coefficient_Wpoly6Gradient = -945.0 / (32.0 * PI * pow9(h_constant));
__constant real_t hSquared_Wpoly6Gradient = h_constant * h_constant;

real3_t Wpoly6Gradient(real3_t diffPosition, real_t radiusSquared)
{
	return (coefficient_Wpoly6Gradient * pow(hSquared_Wpoly6Gradient - radiusSquared, 2)) * diffPosition;
}

__constant real_t coefficient_Wpoly6Laplacian = -945.0 / (32.0 * PI * pow9(h_constant));
__constant real_t hSquared_Wpoly6Laplacian = h_constant * h_constant;

real_t Wpoly6Laplacian(real_t radiusSquared)
{
	return 
		coefficient_Wpoly6Laplacian
		* (hSquared_Wpoly6Laplacian - radiusSquared)
		* (3.0 * hSquared_Wpoly6Laplacian - 7.0 * radiusSquared);
}

__constant real_t coefficientGradient = -45.0 / (PI * pow6(h_constant));

real3_t WspikyGradient(real3_t diffPosition, real_t radiusSquared)
{
	real_t radius = sqrt(radiusSquared);
	return (coefficientGradient * pow(h_constant - radius, 2)) * diffPosition / radius;
}

__constant real_t coefficientLaplacian = 45.0 / (PI * pow6(h_constant));

real_t WviscosityLaplacian(real_t radiusSquared)
{
	real_t radius = sqrt(radiusSquared);
	return coefficientLaplacian * (h_constant - radius);
}

__constant real_t coefficientWpoly6 = 315.0 / (64.0 * PI * pow9(h_constant));
__constant real_t hSquaredWpoly6 = h_constant * h_constant;

real_t Wpoly6(real_t radiusSquared)
{
	return coefficientWpoly6 * pow(hSquaredWpoly6 - radiusSquared, 3);
}

real_t lengthSquared(real3_t vec)
{
	return ((vec.x * vec.x) + (vec.y * vec.y) + (vec.z * vec.z));
}
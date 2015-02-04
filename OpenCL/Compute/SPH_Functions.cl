
real_t W(real3_t ij)
{
	real_t q = length(ij) / h;
	if (q >= 0 || q <= 2)
		q = (2.0 / 3.0) - (9.0 / 8.0)*pow(q, 2) + (19.0 / 24.0)*pow(q, 3) - (5.0 / 32.0)*pow(q, 4);
	else
		q = 0;
	return q;
}

real3_t Wpoly6Gradient(real3_t diffPosition, real_t radiusSquared)
{
	real_t coefficient = -945.0 / (32.0*PI*pow(h, 9));
	real_t hSquared = h*h;

	return (coefficient * pow(hSquared - radiusSquared, 2)) * diffPosition;
}

real_t Wpoly6Laplacian(real_t radiusSquared)
{
	real_t coefficient = -945.0 / (32.0*PI*pow(h, 9));
	real_t hSquared = h*h;

	return coefficient * (hSquared - radiusSquared) * (3.0*hSquared - 7.0*radiusSquared);
}

real3_t WspikyGradient(real3_t diffPosition, real_t radiusSquared)
{
	real_t coefficient = -45.0 / (PI*pow(h, 6));

	real_t radius = sqrt(radiusSquared);

	return (coefficient * pow(h - radius, 2)) * diffPosition / radius;
}

real_t WviscosityLaplacian(real_t radiusSquared)
{
	real_t coefficient = 45.0 / (PI*pow(h, 6));

	real_t radius = sqrt(radiusSquared);

	return coefficient * (h - radius);
}

real_t Wpoly6(real_t radiusSquared)
{
	/*static*/ real_t coefficient = 315.0 / (64.0*PI*pow(h, 9));
	/*static*/ real_t hSquared = h*h;

	return coefficient * pow(hSquared - radiusSquared, 3);
}

real_t lengthSquared(real3_t vec)
{
	return ((vec.x * vec.x) + (vec.y * vec.y) + (vec.z * vec.z));
}
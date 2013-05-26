#include "sphere.hpp"
#include <iostream>

bool Sphere::intersect(Ray const& ray, Intersection* isect) const
{
	Transform const &T = getTransform();

	// Ray to object space
	Ray Robj = transformRay(T, ray);
	Vec RO = Robj.O - Point(0,0,0);

	float dist;

	// Compute A, B and C coefficients
	float a = 1.f;
	float b = 2 * dot(Robj.D, RO);
	float c = dot(RO, RO) - (m_radius * m_radius);
	//std::clog << a << ' ' << b << ' ' << c << std::endl;
	// Find discriminant
	float disc = b*b - 4*a*c;
	// if discriminant is negative there are no real roots, so return 
	// false as ray misses sphere
	if (disc < EPSILON) 
		return false;
	//std::clog << disc << std::endl;
	// compute q as described above
	float discSqrt = sqrt(disc);
	float t1 = (-b + discSqrt)/(2.0f*a);
	if (t1 < EPSILON)
		return false;
	float t0 = (-b - discSqrt)/(2.0f*a);
	if (t0 < EPSILON) {
		dist = t1;
	}
	else {
		dist = t0;
	}
	Point P = Robj.O + dist*Robj.D;
  	Vec N = P - Point(0,0,0);

	if (isect != NULL) {
		isect->P = invTransformPoint(T, P);
		isect->N = invTransformNormal(T, N);
		isect->dist = dist;
		isect->geometry = this;
	}

	return true;
}

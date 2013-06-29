#include "sphere.hpp"
#include <iostream>

bool Sphere::intersect(Ray const& ray, Intersection* isect) const
{
	// Ray in object space
	Vec RO = ray.O - Point(0,0,0);

	float dist;

	// Compute A, B and C coefficients
	float a = 1.f;
	float b = 2 * dot(ray.D, RO);
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

	isect->P = ray.O + dist*ray.D;
	isect->N = (isect->P - Point(0,0,0)).normalized();
	isect->dist = dist;
	isect->geometry = this;

	// u,v coords
	isect->u = 0.5f + atan2f(isect->N.z(), isect->N.x()) / (2 * M_PI);
	isect->v = 0.5f + 2.f * asinf(isect->N.y()) / (2 * M_PI);

	return true;
}

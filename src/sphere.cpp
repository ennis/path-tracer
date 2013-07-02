#include "sphere.hpp"
#include "geometry.hpp"
#include <iostream>

/*
 * Ray in object space
 *
 */ 
bool raySphereIntersection(Ray const& R, LocalGeometry& localGeom)
{
	Vec RO = R.O - Point(0,0,0);

	float dist;

	// Compute A, B and C coefficients
	float a = 1.f;
	float b = 2 * dot(R.D, RO);
	float c = dot(RO, RO) - (m_radius * m_radius);
	//std::clog << a << ' ' << b << ' ' << c << std::endl;
	// Find discriminant
	float disc = b*b - 4*a*c;
	// if discriminant is negative there are no real roots, so return 
	// false as R misses sphere
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

	localGeom.P = R.O + dist * R.D;
	localGeom.N = (localGeom.P - Point(0,0,0)).normalized();
	localGeom.dist = dist;
	localGeom.geometry = this;

	// u,v coords
	localGeom.u = 0.5f + atan2f(localGeom.N.z(), localGeom.N.x()) / (2 * M_PI);
	localGeom.v = 0.5f + 2.f * asinf(localGeom.N.y()) / (2 * M_PI);

	return true;
}

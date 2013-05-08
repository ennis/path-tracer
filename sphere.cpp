#include "sphere.hpp"
#include <iostream>


Sphere::Sphere(float radius_) :
	radius(radius_)
{
}

Sphere::~Sphere()
{
}

bool Sphere::intersectPred(Ray const& r, Point const& pos) const
{
	Vec P,N;
	Vec RO_obj = r.O - pos;
	// Compute A, B and C coefficients
	float a = dot(r.D, r.D);
	float b = 2 * dot(r.D, RO_obj);
	float c = dot(RO_obj, RO_obj) - (radius * radius);
	// Find discriminant
	float disc = b*b - 4*a*c;
	// if discriminant is negative there are no real roots, so return 
	// false as ray misses sphere
	if (disc < EPSILON)
		return false;
	float discSqrt = sqrt(disc);
	float t1 = (-b + discSqrt)/(2.0f*a);
	if (t1 < EPSILON)
		return false;
	return true;
}

bool Sphere::intersect(Ray const& r, Point const& pos, float& dist, Vec& normal) const
{
	//std::clog << r.D << ' ' << r.O << std::endl;
	Vec RO_obj = r.O - pos;
	// Compute A, B and C coefficients
	float a = dot(r.D, r.D);
	float b = 2 * dot(r.D, RO_obj);
	float c = dot(RO_obj, RO_obj) - (radius * radius);
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
	//dist -= EPSILON;
	Point P = r.O + dist*r.D;
  	normal = P - pos;
	return true;
}

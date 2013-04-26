#pragma once
#include "geometry.hpp"

struct Sphere : public Geometry
{
	Sphere(float radius);
	virtual ~Sphere();

	bool intersectPred(Ray const& r, Point const& pos) const;
	bool intersect(Ray const& r, Point const& pos, float& dist, Vec& normal) const;

	float radius;
};

#pragma once
#include "geometry.hpp"

struct Plane : public Geometry
{
	Plane(Vec normal_) : N(normal_.normalized())
	{}

	virtual ~Plane()
	{}

	bool intersectPred(Ray const& r, Point const& pos) const {
		return true;
	}
	bool intersect(Ray const& r, Point const& pos, float& dist, Vec& normal) const;

	Vec N;
	
};

bool rayPlaneIntersection(Ray const& r, Vec const& N, Point const& P, float& dist);
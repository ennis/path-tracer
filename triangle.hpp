#pragma once
#include "geometry.hpp"

struct Triangle : public Geometry
{
	Triangle(Point const& P1_, Point const& P2_, Point const& P3_) : 
		Geometry(identityM, identityM),
		P1(P1_),
		P2(P2_),
		P3(P3_)
	{
		PX = P2 - P1;
		PY = P3 - P1;
		N = cross(PY, PX).normalized();
	}

	// In world space
	bool intersectPred(Ray const& r, Point const& pos) const {
		return true;
	}
	// In world space
	bool intersect(Ray const& r, Point const& pos, float& dist, Vec& normal) const;

	Point P1, P2, P3;
	Vec N, PX, PY;
};
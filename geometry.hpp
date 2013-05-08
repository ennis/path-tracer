#pragma once
#include "vec.hpp"
#include "matrix4x4.hpp"

struct Geometry
{
	// In world space
	virtual bool intersectPred(Ray const& r, Point const& pos) const = 0;
	// In world space
	virtual bool intersect(Ray const& r, Point const& pos, float& dist, Vec& normal) const = 0;
};

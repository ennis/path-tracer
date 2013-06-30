#pragma once
#include "vec.hpp"
#include "transform.hpp"
#include "bbox.hpp"
#include "intersection.hpp"
#include "localgeom.hpp"

class Geometry
{
public:
	Geometry()
	{}

	// Ray in object space
	virtual bool intersect(Ray const& ray, Intersection* isect) const = 0;

	virtual AABB getAABB() const {
		return AABB();
	}


protected:
};


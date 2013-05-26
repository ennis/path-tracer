#pragma once
#include "vec.hpp"
#include "transform.hpp"
#include "bbox.hpp"

class Geometry;

struct Intersection
{
	Geometry const *geometry;
	// Distance from ray origin to intersection point
	float dist;
	// Intersection point
	Point P;
	// u,v coordinates
	float u, v;
	// normal, tangent
	Vec N;
};


class Geometry
{
public:
	Geometry(Transform const& w2o) : m_w2o(w2o)
	{}

	// In world space
	virtual bool intersect(Ray const& ray, Intersection* isect) const = 0;

	virtual AABB getAABB() const {
		return AABB();
	}

	virtual Transform const& getTransform() const {
		return m_w2o;
	}

protected:
	Transform m_w2o;
};

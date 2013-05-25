#pragma once
#include "vec.hpp"
#include "matrix4x4.hpp"
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
	Geometry(Matrix4x4 const &w2o, Matrix4x4 const& invw2o) : m_w2o(w2o), m_invw2o(invw2o)
	{}

	// In world space
	virtual bool intersect(Ray const& ray, Intersection* isect) const = 0;

	virtual AABB getAABB() const {
		return AABB();
	}

	virtual Matrix4x4 const& getTransform() const {
		return m_w2o;
	}

	virtual Matrix4x4 const& getInvTransform() const {
		return m_invw2o;
	}

protected:
	Matrix4x4 m_w2o, m_invw2o;
};

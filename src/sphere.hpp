#pragma once
#include "geometry.hpp"

class Sphere : public Geometry
{
public:
	Sphere(Transform const& w2o, float radius) : Geometry(w2o), m_radius(radius)
	{
		m_aabb = transformAABB(w2o, AABB(Point(-radius, -radius, -radius), Point(radius, radius, radius)));
	}

	virtual ~Sphere()
	{}

	virtual bool intersect(Ray const& ray, Intersection* isect) const;

	virtual AABB getAABB() const {
		return m_aabb;
	}

	virtual Point getCenter() const {
		return Point(0,0,0) + MGetTranslation(m_w2o.m_t);
	}

	virtual float getRadius() const {
		return m_radius;
	}

private:
	AABB m_aabb;
	float m_radius;
};

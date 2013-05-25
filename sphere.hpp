#pragma once
#include "geometry.hpp"

class Sphere : public Geometry
{
public:
	Sphere(Matrix4x4 const& w2o, Matrix4x4 const& invw2o, float radius) : Geometry(w2o, invw2o), m_radius(radius)
	{
		m_aabb = transformAABB(w2o, AABB(Point(-radius, -radius, -radius), Point(radius, radius, radius)));
	}

	virtual ~Sphere()
	{}

	virtual bool intersect(Ray const& ray, Intersection* isect) const;

	virtual AABB getAABB() const {
		return m_aabb;
	}

	virtual Vec getCenter() const {
		return MGetTranslation(m_w2o);
	}

private:
	AABB m_aabb;
	float m_radius;
};

#pragma once
#include "geometry.hpp"

class Plane : public Geometry
{
public:
	Plane(Vec const& normal, Point const& pos) : Geometry(identityM, identityM), m_normal(normal.normalized()), m_pos(pos)
	{}

	virtual ~Plane()
	{}
	
	virtual bool intersect(Ray const& ray, Intersection* isect) const;

private:
	Vec m_normal;
	Point m_pos;
};

bool rayPlaneIntersection(Ray const& ray, Vec const& normal, Point const& point, float& dist);
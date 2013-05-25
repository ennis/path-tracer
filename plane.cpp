#include "plane.hpp"
#include <iostream>
#include <cmath>

bool rayPlaneIntersection(Ray const& ray, Vec const& normal, Point const& point, float& dist)
{
	if (fabs(dot(ray.D, normal)) < EPSILON) {
		return false;
	} else {
		dist = dot(point - ray.O, normal) / dot(ray.D, normal);
		if (dist < EPSILON) {
			return false;
		}
		return true;
	}
}

bool Plane::intersect(Ray const& ray, Intersection* isect) const
{
	float dist;
	if (rayPlaneIntersection(ray, m_normal, m_pos, isect->dist)) {
		isect->N = m_normal;
		isect->P = ray.O + isect->dist*ray.D;
		isect->geometry = this;
		return true;
	} else {
		return false;
	}
}

#pragma once
#include "vec.hpp"
#include "ray.hpp"
#include "matrix4x4.hpp"

struct AABB
{
	AABB()
	{
		sx0 = sx1 = sy0 = sy1 = sz0 = sz1 = 0;
	}

	AABB(Vec const& lowerleft, Vec const& upperright)
	{
		sx0 = std::min(lowerleft.x(), upperright.x());
		sx1 = std::max(lowerleft.x(), upperright.x());
		sy0 = std::min(lowerleft.y(), upperright.y());
		sy1 = std::max(lowerleft.y(), upperright.y());
		sz0 = std::min(lowerleft.z(), upperright.z());
		sz1 = std::max(lowerleft.z(), upperright.z());
	}

	float sx0;
	float sx1;
	float sy0;
	float sy1;
	float sz0;
	float sz1;
};

bool rayAABBIntersect(Ray const& ray, AABB const& aabb, float& near, float& far);
//AABB transformAABB(Transform const&

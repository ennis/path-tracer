#include "bbox.hpp"

using namespace std;

bool rayAABBIntersect(Ray const& ray, AABB const& aabb, float& near, float& far)
{
	// slab X
	float t0x = (aabb.sx0 - ray.O.x()) / ray.D.x();	// distance to intersection with nearest plane from slab
	float t1x = (aabb.sx1 - ray.O.x()) / ray.D.x();	// same, with the far plane
	if (t0x > t1x) {
		swap(t0x, t1x);
	}
	if (t1x < 0) {
		return false;
	}

	// slab Y
	float t0y = (aabb.sy0 - ray.O.y()) / ray.D.y();
	float t1y = (aabb.sy1 - ray.O.y()) / ray.D.y();
	if (t0y > t1y) {
		swap(t0y, t1y);
	}
	if (t1y < 0) {
		return false;
	}

	// slab Z
	float t0z = (aabb.sz0 - ray.O.z()) / ray.D.z();
	float t1z = (aabb.sz1 - ray.O.z()) / ray.D.z();
	if (t0z > t1z) {
		swap(t0z, t1z);
	}
	if (t1z < 0) {
		return false;
	}

	float tfar = min(min(t1x, t1y), t1z);
	float tnear = max(max(t0x, t0y), t0z);

	if (tnear > tfar) {
		return false;
	}
	if (tfar < 0) {
		return false;
	}

	near = tnear;
	far = tfar;
	return true;
}

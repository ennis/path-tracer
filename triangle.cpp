#include "triangle.hpp"
#include "plane.hpp"
#include <iostream>

bool Triangle::intersect(Ray const& r, Point const& pos, float& dist, Vec& normal) const
{
	if (!rayPlaneIntersection(r, N, P1, dist)) {
		//std::clog << P1 << '\n';
		return false;
	}
	Point P = r.O + dist * r.D;
	// check if the intersection point lies inside the plane
	Vec P1P = P - P1;
	float cx = dot(PX, P1P);
	float cy = dot(PY, P1P);
	//std::clog << cx << ' ' << cy << '\n';
	if (0.f < cx && cx < 1.f && 0.f < cy && cy < 1.f) {
		if ((1.f - (cx + cy)) > 0.f) {
			normal = N;
			return true;
		} else {
			return false;
		}
	}
	else {
		//std::clog << "NI2";
		return false;
	}
}

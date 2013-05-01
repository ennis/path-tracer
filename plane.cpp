#include "plane.hpp"
#include <iostream>
#include <cmath>

bool rayPlaneIntersection(Ray const& r, Vec const& N, Point const& P, float& dist)
{
	if (fabs(dot(r.D, N)) < EPSILON) {
		// XXX line inside plane?
		//std::clog << r.D << ' ' << N << '\n';
		return false;
	} else {
		//std::clog << "P:" << P << '\n';
		//std::clog << "r.O:" << r.O << '\n';
		//std::clog << "diff:" << P - r.O << '\n';
		dist = dot(P - r.O, N) / dot(r.D, N);
		//std::clog << dist << '\n';
		if (dist < EPSILON) {
			return false;
		}
		return true;
	}
}

bool Plane::intersect(Ray const& r, Point const& pos, float& dist, Vec& normal) const
{
	if (rayPlaneIntersection(r, N, pos, dist)) {
		normal = N;
		return true;
	} else {
		return false;
	}
}

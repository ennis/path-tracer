#include "plane.hpp"
#include <iostream>
#include <cmath>

bool Plane::intersect(Ray const& r, Point const& pos, float& dist, Vec& normal) const
{
	if (fabs(dot(r.D, N)) < EPSILON) {
		// XXX line inside plane?
		//std::clog << r.D << ' ' << N << '\n';
		return false;
	} else {
		//std::clog << "pos:" << pos << '\n';
		//std::clog << "r.O:" << r.O << '\n';
		//std::clog << "diff:" << pos - r.O << '\n';
		dist = dot(pos - r.O, N) / dot(r.D, N);
		if (dist < EPSILON) {
			return false;
		}
		//std::clog << dist << '\n';
		normal = N;
		return true;
	}
}

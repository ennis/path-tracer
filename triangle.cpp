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

	Vec U = P2 - P1;
	Vec V = P3 - P1;

	float uu, uv, vv, wu, wv, D;
    uu = dot(U,U);
    uv = dot(U,V);
    vv = dot(V,V);
    Vec W = P - P1;
    wu = dot(W,U);
    wv = dot(W,V);
    D = uv * uv - uu * vv;

    // get and test parametric coords
    float s, t;
    s = (uv * wv - vv * wu) / D;
    if (s < 0.0 || s > 1.0)         // I is outside T
        return false;
    t = (uv * wu - uu * wv) / D;
    if (t < 0.0 || (s + t) > 1.0)  // I is outside T
        return false;

	return true;
}

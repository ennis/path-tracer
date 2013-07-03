#ifndef RAY_HPP
#define RAY_HPP

#include "vec.hpp"

struct Ray {
	Ray() 
	{}

	Ray(Point const& origin, 
		Vec const& direction) : 
	O(origin), 
	D(direction.normalized())
	{}

	Point O;
	Vec D;
};

Vec reflectedRay(Vec const& N, Vec const& I);
Vec refractedRay(Vec const& N, Vec const& I, float n);
Vec scatteredReflectedRay(Vec const& N, Vec const& I);
Vec specularRay(Vec const& N, Vec const& I, float specular, bool& bounce);
Vec uniformRandomRay(Vec const& N);
Vec halfway(Vec const& in, Vec const& out);

Vec cosineSampleHemisphere();
Vec uniformSampleHemisphere();
Vec perfectSpecularReflection(Vec const& I);
Vec cosinePowerSampleHemisphere(float exp);

static bool rayPlaneIntersection(Ray const& R, Vec const& N, Vec const& P, float& dist)
{
	if (fabs(dot(R.D, N)) < EPSILON) {
		return false;
	} else {
		dist = dot(P - R.O, N) / dot(R.D, N);
		if (dist < EPSILON) {
			return false;
		}
		return true;
	}
}

#endif
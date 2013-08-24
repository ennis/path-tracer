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

	Point along(float dist) const
	{
		return O + dist * D;
	}

	Point O;
	Vec D;
};


Vec reflectedRay(Vec const& N, Vec const& I);
Vec refractedRay(Vec const& N, Vec const& I, float n);
Vec scatteredReflectedRay(Vec const& N, Vec const& I);
Vec specularRay(Vec const& N, Vec const& I, float specular, bool& bounce);
Vec uniformRandomRay(Vec const& N);
Vec halfway(Vec const& in, Vec const& out);

Vec cosineSampleHemisphere(float u1, float u2);
Vec uniformSampleHemisphere(float u1, float u2);
Vec perfectSpecularReflection(Vec const& I);
Vec cosinePowerSampleHemisphere(float exp, float u1, float u2);
Vec perfectSpecularRefraction(Vec const& I, float n);

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

/*static Vec sampleCone(float cosThetaMax)
{
	float s1 = frand(0.f, 1.f);	// TODO samplers
	float s2 = frand(0.f, 1.f);

	float phi = 2 * M_PI * s1;
	float v = 1.f - s2 * (1 - cos_max);
	float w = sqrt(1 - v*v);
	Vec Rd = cos(phi) * w * T + sin(phi) * w * S + v * D.normalized();
	Rd = Rd.normalized();
}
*/

#endif
#pragma once
#include "bsdf.hpp"
#include "ray.hpp"
#include <cmath>
#include "util.hpp"

static inline float fresnelCoef(Vec const& H, Vec const& in, Vec const& N, float R0)
{
	float c = 1 - dot(H, in);
	return R0 + (1-R0)*c*c*c*c*c;
}

class SchlickBSDF : public BSDF
{
public:
	SchlickBSDF(float R0) : m_r0(R0)
	{}

	Vec sample(Vec const& N, Vec const& in, Vec const& color, float sampleX, float sampleY, Vec& out, bool& specular) const
	{
		float R = fresnelCoef(N, in, N, m_r0);

		if (frand(0,1) < R) {
			out = reflectedRay(N, in);
			specular = true;
			return Vec(1.f,1.f,1.f);
		}
		else {
			out = sampleCosineWeightedRay(N, sampleX, sampleY);
			specular = false;
			return eval(N, in, out, color);
		}
	}

	Vec eval(Vec const& N, Vec const& in, Vec const& out, Vec const& color) const
	{
		  //float c = dot(N, out);
		  return Vec(1.f,1.f,1.f);
	}

	bool isSpecular() const {
		return false;
	}

private:
	float m_r0;
};

#pragma once
#include "bsdf.hpp"
#include "ray.hpp"
#include <cmath>
#include "util.hpp"

class SchlickBSDF : public BSDF
{
public:
	SchlickBSDF(float R0) : m_r0(R0)
	{}

	Vec sample(Vec const& in, Vec const& N, Vec& out, bool& specular, float sampleX, float sampleY) const
	{
		out = reflectedRay(N, in);
		Vec H = halfway(in, out);
		float c = 1 - dot(H, out);
		float R = m_r0 + (1-m_r0)*c*c*c*c*c;

		if (frand(0,1) < R) {
			specular = true;
			return Vec(1.f,1.f,1.f);
		}
		else {
			out = sampleCosineWeightedRay(N, sampleX, sampleY);
			specular = false;
			return eval(in, out, N);
		}
	}

	Vec eval(Vec const& in, Vec const& out, Vec const& N) const
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
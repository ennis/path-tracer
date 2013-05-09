#pragma once
#include "bsdf.hpp"
#include "ray.hpp"
#include <cmath>

class SchlickBSDF : public BSDF
{
public:
	SchlickBSDF(float R0) : R0_(R0)
	{}

	Vec sample(Vec const& in, Vec const& N, Vec& out, float sampleX, float sampleY) const
	{
		out = sampleCosineWeightedRay(N, sampleX, sampleY);
		return eval(in, out, N);
	}

	Vec eval(Vec const& in, Vec const& out, Vec const& N) const
	{
		  Vec H = halfway(in, out);
		  float c = 1 - dot(H, out);
		  float R = R0_ + (1-R0_)*c*c*c*c*c;
		  return Vec(R,R,R);
	}

	bool isSpecular() const {
		return false;
	}

private:
	float R0_;
};
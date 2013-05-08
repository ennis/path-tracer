#pragma once
#include "bsdf.hpp"
#include "ray.hpp"
#include <iostream>
#include <algorithm> 
#include <cmath>

class PhongBSDF : public BSDF
{
public:
	PhongBSDF(float phongCoef) : phongCoef_(phongCoef)
	{}

	Vec sample(Vec const& in, Vec const& N, Vec& out, float sampleX, float sampleY) const
	{
		out = sampleCosineWeightedRay(N, sampleX, sampleY);
		return eval(in, out, N);
	}

	Vec eval(Vec const& in, Vec const& out, Vec const& N) const
	{
		Vec H = halfway(in, out);
		float s = powf(dot(N,H), phongCoef_);
		return Vec(s,s,s);
	}

	bool needSamples() const {
		return true;
	}

private:
	float phongCoef_;
};
#pragma once
#include "bsdf.hpp"
#include "ray.hpp"
#include <iostream>
#include <algorithm> 
#include <cmath>
#include "util.hpp"

class PhongBSDF : public BSDF
{
public:
	PhongBSDF(float phongCoef) : phongCoef_(phongCoef)
	{}

	Vec sample(Vec const& N, Vec const& in, Vec const& color, float sampleX, float sampleY, Vec& out, bool& specular) const
	{
		out = sampleCosineWeightedRay(N, sampleX, sampleY);
		specular = false;
		return eval(N, in, out, color);
	}

	Vec eval(Vec const& N, Vec const& in, Vec const& out, Vec const& color) const
	{
		Vec H = halfway(in, out);
		float s = powf(dot(N,H), phongCoef_);
		return Vec(s,s,s);
	}

	bool isSpecular() const {
		return false;
	}

private:
	float phongCoef_;
};
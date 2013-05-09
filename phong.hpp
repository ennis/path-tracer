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

	Vec sample(Vec const& in, Vec const& N, Vec& out, bool& specular, float sampleX, float sampleY) const
	{
		if (frand(0,1) < 0.5f) {
			out = sampleCosineWeightedRay(N, sampleX, sampleY);
			specular = false;
			return eval(in, out, N);
		} else {
			out = reflectedRay(N, in);
			specular = true;
			return Vec(1,1,1);
		}		
	}

	Vec eval(Vec const& in, Vec const& out, Vec const& N) const
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
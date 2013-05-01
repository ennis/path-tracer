#pragma once
#include "bsdf.hpp"
#include "ray.hpp"
#include <cmath>

// Diffuse BSDF
class DiffuseBSDF : public BSDF
{
public:
	DiffuseBSDF()
	{}

	Vec sample(Vec const& in, Vec const& N, Vec& out, float sampleX, float sampleY) const
	{
		out = sampleCosineWeightedRay(N, sampleX, sampleY);
		//std::clog << dot(N,in) <<'\n';
		return Vec(1.f, 1.f, 1.f);
	}

	Vec eval(Vec const& in, Vec const& out, Vec const& N) const
	{
		return Vec(1.f, 1.f, 1.f);
	}

	bool needSamples() const {
		return true;
	}

private:
};
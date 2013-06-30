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

	Vec sample(Vec const& N, Vec const& in, Vec const& color, float sampleX, float sampleY, Vec& out, bool& specular) const
	{
		specular = false;
		out = sampleCosineWeightedRay(N, sampleX, sampleY);
		//std::clog << dot(N,in) <<'\n';
		return Vec(1.f, 1.f, 1.f);
	}

	Vec eval(Vec const& N, Vec const& in, Vec const& out, Vec const& color) const
	{
		return Vec(1.f, 1.f, 1.f);
	}

private:
};
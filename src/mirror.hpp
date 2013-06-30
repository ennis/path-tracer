#pragma once
#include "bsdf.hpp"
#include "ray.hpp"

class MirrorBSDF : public BSDF
{
public:
	MirrorBSDF() 
	{}

	Vec sample(Vec const& N, Vec const& in, Vec const& color, float sampleX, float sampleY, Vec& out, bool& specular) const
	{
		// generate ray in hemisphere
	    out = reflectedRay(N, in);
		specular = true;
		return Vec(1.f, 1.f, 1.f);
	}

	Vec eval(Vec const& N, Vec const& in, Vec const& out, Vec const& color) const
	{
		return Vec(0.f, 0.f, 0.f);
	}

private:
};
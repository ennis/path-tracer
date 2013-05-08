#pragma once
#include "bsdf.hpp"
#include "ray.hpp"

class MirrorBSDF : public BSDF
{
public:
	MirrorBSDF() 
	{}

	Vec sample(Vec const& in, Vec const& N, Vec& out, float sampleX, float sampleY) const
	{
		// generate ray in hemisphere
	    out = reflectedRay(N, in);
		return Vec(1.f, 1.f, 1.f);
	}

	Vec eval(Vec const& in, Vec const& out, Vec const& N) const
	{
		return Vec(0.f, 0.f, 0.f);
	}

	bool needSamples() const {
		return false;
	}

private:
};
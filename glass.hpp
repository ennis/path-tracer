#pragma once
#include "bsdf.hpp"
#include "ray.hpp"

class GlassBSDF : public BSDF
{
public:
	GlassBSDF(float refractionIndex) : m_refractionIndex(refractionIndex)
	{}

	Vec sample(Vec const& in, Vec const& N, Vec& out, float sampleX, float sampleY) const
	{
		// generate ray in hemisphere
		if (dot(in,N) < 0) {
	    	out = refractedRay(N, in, m_refractionIndex);
	    }
	    else {
			out = refractedRay(-1.0*N, in, 1.f/m_refractionIndex);
	    }
		return Vec(1.f, 1.f, 1.f);
	}

	Vec eval(Vec const& in, Vec const& out, Vec const& N) const
	{
		return Vec(1.f, 1.f, 1.f);
	}

	bool needSamples() const {
		return false;
	}

private:
	float m_refractionIndex;
};
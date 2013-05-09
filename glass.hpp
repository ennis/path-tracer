#pragma once
#include "bsdf.hpp"
#include "ray.hpp"

class GlassBSDF : public BSDF
{
public:
	GlassBSDF(float refractionIndex, float reflection = 0.5f) : m_refractionIndex(refractionIndex), m_reflection(reflection)
	{}

	Vec sample(Vec const& in, Vec const& N, Vec& out, float sampleX, float sampleY) const
	{
		// generate ray in hemisphere
		if (dot(in,N) > 0) {
			if (sampleX > m_reflection) {
	    		out = refractedRay(N, in, m_refractionIndex);
			}
			else {
				out = reflectedRay(N, in);
			}
		}
		else {
			out = refractedRay(-1.0f*N, in, 1.f/m_refractionIndex);
		}
		return Vec(1.f, 1.f, 1.f);
	}

	Vec eval(Vec const& in, Vec const& out, Vec const& N) const
	{
		return Vec(1.f, 1.f, 1.f);
	}

	bool isSpecular() const {
		return true;
	}

private:
	float m_refractionIndex;
	float m_reflection;
};
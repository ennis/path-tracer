#pragma once
#include "bsdf.hpp"
#include "ray.hpp"
#include "schlick.hpp"

class GlassBSDF : public BSDF
{
public:
	GlassBSDF(float refractionIndex) : m_refractionIndex(refractionIndex)
	{
		m_r0 = (1-m_refractionIndex)/(1+m_refractionIndex);
		m_r0 *= m_r0;
	}

	Vec sample(Vec const& N, Vec const& in, Vec const& color, float sampleX, float sampleY, Vec& out, bool& specular) const
	{
		specular = true;

		if (dot(in,N) > 0) {
			float R = fresnelCoef(N, in, N, m_r0);
			if (sampleX < R) {
				out = reflectedRay(N, in);
			} else {
	    		out = refractedRay(N, in, m_refractionIndex);
			}
		}
		else {
			out = refractedRay(-1.0f*N, in, 1.f/m_refractionIndex);
		}
		
		return Vec(1.f, 1.f, 1.f);
	}

	Vec eval(Vec const& N, Vec const& in, Vec const& out, Vec const& color) const
	{
		return Vec(1.f, 1.f, 1.f);
	}

private:
	float m_refractionIndex;
	float m_r0;
};
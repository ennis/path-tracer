#pragma once
#include "bsdf.hpp"
#include "ray.hpp"
#include <iostream>
#include <algorithm> 

class SpecularBSDF : public BSDF
{
public:
	SpecularBSDF(float specularCoef) : specularCoef_(specularCoef)
	{}

	Vec sample(Vec const& in, Vec const& N, Vec& out, bool& specular, float sampleX, float sampleY) const
	{
		// generate ray in hemisphere
		bool bounce;
		//Vec R = reflectedRay(N, in);
		out = sampleSpecularRay(N, in, specularCoef_, sampleX, sampleY);
		if (dot(N,out) < 0.f) {
			return Vec(0.f, 0.f, 0.f);
		}
		specular = true;
		return Vec(1.f, 1.f, 1.f);
	}

	Vec eval(Vec const& in, Vec const& out, Vec const& N) const
	{
		Vec R = reflectedRay(N, in);
		
		return Vec(1.f, 1.f, 1.f)*std::max(0.f,dot(out,R));
	}

	bool isSpecular() const {
		return true;
	}

private:
	float specularCoef_;
};
#pragma once
#include "ray.hpp"

class BSDF
{
public:
	virtual Vec sample(Vec const& in, Vec const& N, Vec& out, float sampleX, float sampleY) const = 0;
	virtual Vec eval(Vec const& in, Vec const& out, Vec const& N) const = 0;
	virtual bool isSpecular() const = 0;
};
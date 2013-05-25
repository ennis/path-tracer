#pragma once
#include "ray.hpp"
#include "geometry.hpp"

class BSDF
{
public:
	virtual Vec sample(Vec const& N, Vec const& in, Vec const& color, float sampleX, float sampleY, Vec& out, bool& specular) const = 0;
	virtual Vec eval(Vec const& N, Vec const& in, Vec const& out, Vec const& color) const = 0;
	//virtual bool isSpecular() const = 0;
};
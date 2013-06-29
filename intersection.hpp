#pragma once
#include "geometry.hpp"
#include "bsdf.hpp"
#include "texture.hpp"

struct Intersection
{
	// BSDF at hit point
	BSDF const *bsdf;
	// Texture object
	Texture const* texture;
	// Local geometry
	LocalGeometry localGeometry;
};

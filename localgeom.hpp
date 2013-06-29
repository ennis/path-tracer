#pragma once
#include "vec.hpp"

class Geometry;

// All vectors in world space
struct LocalGeometry
{
	// geometry object at hit point
	Geometry const *geometry;
	// hit point
	Point P;
	// Local base
	Vec N, T, S;
	// Texture coordinates
	float u, v;
};

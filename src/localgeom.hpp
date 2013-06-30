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

static inline Vec worldToLocal(
					LocalGeometry const& localGeom, 
					Vec const& VW)
{
	return Vec(dot(localGeom.T,VW), dot(localGeom.S,VW), dot(localGeom.N,VW));
}

static inline Vec localToWorld(
					LocalGeometry const& localGeom,
					Vec const& VL)
{
	return VL.x() * localGeom.T + 
			VL.y() * localGeom.S +
			VL.z() * localGeom.N;
}
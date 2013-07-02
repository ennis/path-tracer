#pragma once
#include "vec.hpp"

class Primitive;

//==================================
// LocalGeometry
struct LocalGeometry
{
	// primitive at hit point
	Primitive const *primitive;
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
	return Vec(dot(localGeom.T,VW), 
				dot(localGeom.S,VW),
				dot(localGeom.N,VW));
}

static inline Vec localToWorld(
					LocalGeometry const& localGeom,
					Vec const& VL)
{
	return VL.x() * localGeom.T + 
			VL.y() * localGeom.S +
			VL.z() * localGeom.N;
}

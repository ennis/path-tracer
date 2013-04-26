#pragma once
#include "geometry.hpp"


struct Triangle : public Geometry
{
	Triangle(Point const& P1_, Point const& P2_, Point const& P3) : 
		P1(P1_),
		P2(P2_),
		P3(P3_)
	{}

	// In world space
	virtual bool intersectPred(Ray const& r, Point const& pos) const = 0;
	// In world space
	virtual bool intersect(Ray const& r, Point const& pos, float& dist, Vec& normal) const = 0;	
};
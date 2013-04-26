#pragma once
#include "vec.hpp"
#include "material.hpp"
#include "matrix4x4.hpp"
#include "bbox.hpp"
#include "geometry.hpp"

/*
 Object: material + geometry
 */

struct Object
{
	Point position;
	Vec color;
	Vec emittance;
	Geometry *geometry;
	Material *material;
	//Transform *transform;
	//Texture *texture;
};

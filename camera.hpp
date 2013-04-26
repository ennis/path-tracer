#pragma once
#include "vec.hpp"
#include "matrix4x4.hpp"
#include "ray.hpp"

struct Camera 
{
	Point eye;
	Point lookAt;
	Vec up;
	float width;
	float height;
	float screenDist;
	Matrix4x4 viewM;
};

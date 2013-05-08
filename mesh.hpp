#pragma once
#include "vec.hpp"
#include <vector>

class Mesh 
{
public:

	Mesh() : vertices(NULL),
			 triangles(NULL),
			 normals(NULL)
	{}

	static Mesh loadFromOBJ(char const* filename);

private:
	float *vertices;
	struct {
		int p1, p2, p3;
	} *triangles;
	struct {
		float x, y, z;
	} *normals;
};
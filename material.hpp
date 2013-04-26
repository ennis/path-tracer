#ifndef MATERIAL_HPP
#define MATERIAL_HPP 
#include "vec.hpp"
#include "ray.hpp"
 
/* Material: color, reflection type (ray generation) */ 

enum ReflectionType
{
	R_REFLECTIVE,
	R_REFRACTIVE,
	R_DIFFUSE,
	R_SPECULAR
};

enum BRDFType
{
	DUMMY,
	DIFFUSE_STD,
	DIFFUSE_SCHLICK,
	GAUSSIAN,
	SINC,
};

struct Material
{
	int reflectionType; 
	float specular;
	float refractiveIndex;

	float reflectionP;
	float refractionP;
	float diffuseP;

	BRDFType brdfType;
	// BRDF parameters
	union {
		float sincCoef;
		float R0;
	} brdfData;
};

Vec evalBRDF(Material const& mat, Vec const& in, Vec const& out, Vec const& normal);

// for diffuse materials
Vec stdBrdf(Material const& mat, Vec const& in, Vec const& out, Vec const& normal);
Vec schlickBrdf(Material const& mat, Vec const& in, Vec const& out, Vec const& normal);
Vec dummyBrdf(Material const& mat, Vec const& in, Vec const& out, Vec const& normal);
Vec gaussianBrdf(Material const& mat, Vec const& in, Vec const& out, Vec const& normal);
Vec sincBrdf(Material const& mat, Vec const& in, Vec const& out, Vec const& normal);

#endif
#include "material.hpp"
#include "renderer.hpp"
#include "util.hpp"
#include <cstdlib>
#include <iostream>


Vec evalBRDF(Material const& mat, Vec const& in, Vec const& out, Vec const& normal)
{
	switch (mat.brdfType)
	{
	case DIFFUSE_STD:
		return stdBrdf(mat, in, out, normal);
	case DIFFUSE_SCHLICK:
		return schlickBrdf(mat, in, out, normal);
	case GAUSSIAN:
		return gaussianBrdf(mat, in, out, normal);
	case SINC:
		return sincBrdf(mat, in, out, normal);
	case DUMMY:
	default:
		return dummyBrdf(mat, in, out, normal);
	}
}


// for diffuse materials
Vec stdBrdf(Material const& mat, Vec const& in, Vec const& out, Vec const& normal)
{
	Vec H = halfway(in, out);
	float alpha = dot(H, normal);
	float R = mat.brdfData.R0 + pow(alpha, mat.specular);
	return Vec(R,R,R);
}

// for diffuse materials
Vec schlickBrdf(Material const& mat, Vec const& in, Vec const& out, Vec const& normal)
{
  Vec H = halfway(in, out);
  float c = 1 - abs(dot(H, out));
  float R0 = mat.brdfData.R0;
  float R = R0 + (1-R0)*c*c*c*c*c;
  return Vec(R,R,R);
}

Vec dummyBrdf(Material const& mat, Vec const& in, Vec const& out, Vec const& normal)
{
  return Vec(1.f,1.f,1.f);
}

Vec gaussianBrdf(Material const& mat, Vec const& in, Vec const& out, Vec const& normal)
{
  float alpha = dot(normal, out);
  float sigma = 0.1;
  float sqs = sigma*sigma;
  float R = mat.brdfData.R0 + (1/sqrt(2*M_PI*sqs))*exp(-(alpha-1)*(alpha-1)/(2*sqs));
  return Vec(R,R,R); 
}

Vec sincBrdf(Material const& mat, Vec const& in, Vec const& out, Vec const& normal)
{
  float alpha = acos(dot(normal, out));
  float omega = 0.1;
  float R0 = mat.brdfData.R0;
  // remove singularities
  float R = (alpha*omega < EPSILON) ? R0 : (0.4 + sin(omega*alpha)/(omega*alpha));
  return Vec(R,R,R); 
}
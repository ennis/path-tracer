#include <iostream>
#include <fstream>
#include <cmath>
#include "bsdf.hpp"
#include "vec.hpp"
#include "ray.hpp"

#define NUM_SAMPLES 1000

static const char *testResultFileNameUniform = "lambertian_uniform.out";
static const char *testResultFileNameSlice = "lambertian_slice.out";

int main(int argc, char ** argv)
{

	BxDF *testBxdf = new LambertianBRDF;
	Vec const color = Vec(1.f,0.f,0.f);

	// sampling directions
	Vec *samplingDirs = new Vec[NUM_SAMPLES];
	// results
	float *results = new float[NUM_SAMPLES];

	for (int i = 0; i < NUM_SAMPLES; ++i) {
		samplingDirs[i] = uniformSampleHemisphere();
	}

	// test vector
	Vec Wi = Vec(0.f,1.f,1.f).normalized();
	Vec bxdfResult;
	float pdfResult;

	//=====================================
	// Uniform sampling test
	std::ofstream resultFileUniform(testResultFileNameUniform);
	resultFileUniform << "# Wi=" << 
			Wi.x() << ' ' << 
			Wi.y() << ' ' << 
			Wi.z() << '\n';
	resultFileUniform << "# Wo.x Wo.y Wo.z value\n";

	// sample along incoming ray direction
	for (int i = 0; i < NUM_SAMPLES; ++i) 
	{
		testBxdf->eval(color, Wi, samplingDirs[i], bxdfResult, pdfResult);
		//results[i] = bxdfResult.x();
		resultFileUniform << samplingDirs[i].x() << ' ' << 
					samplingDirs[i].y() << ' ' << 
					samplingDirs[i].z() << ' ' << 
					bxdfResult.x() << '\n'; 
	}

	resultFileUniform.close();

	//=====================================
	// sampling test along incoming direction

	std::ofstream resultFileSlice(testResultFileNameSlice);
	resultFileSlice << "# Wi=" << 
			Wi.x() << ' ' << 
			Wi.y() << ' ' << 
			Wi.z() << '\n';
	resultFileSlice << "# theta value\n";
	for (float theta = -M_PI/2.f; theta < M_PI/2.f; theta += M_PI / static_cast<float>(NUM_SAMPLES))
	{
		Vec Wo = Vec(0.f, sinf(theta), cosf(theta));
		testBxdf->eval(color, Wi, Wo, bxdfResult, pdfResult);
		resultFileSlice << theta << ' ' << bxdfResult.x() << '\n';
	}


	delete [] samplingDirs;
	delete [] results;
	delete testBxdf;
	
	return 0;
}
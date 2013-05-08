#pragma once
#include <vector>
#include "camera.hpp"
#include "vec.hpp"
#include "ray.hpp"
#include "object.hpp"
#include <cstdint>

static const unsigned int DEFAULT_MAX_DEPTH = 5;
static const unsigned int DEFAULT_SPP = 10;


struct RenderState
{
	// parameters
	std::vector<Object*> scene;
	std::vector<Object*> lightSources;
	unsigned int samplesPerPixel;
	unsigned int maxDepth;
	Camera camera;
	unsigned int pixelWidth;
	unsigned int pixelHeight;
	uint32_t *buffer;

	bool supersampling;
	bool cosineWeightedSampling;
	
	unsigned int numRenderedLines;
	unsigned int numSamples;
};

void render(RenderState& rs);

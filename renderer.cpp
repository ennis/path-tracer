#include "renderer.hpp"
#include <iostream>
#include <iomanip>

// x9 supersampling
static const unsigned int NUM_SUPERSAMPLES = 9;
static const float SS_EPSILON = 0.33f;
static float ssdisp[9][2] = {
	{-SS_EPSILON, -SS_EPSILON},
	{0.0, -SS_EPSILON},
	{SS_EPSILON, -SS_EPSILON},
	{-SS_EPSILON, 0.0},
	{0.0, 0.0},
	{SS_EPSILON, 0.0},
	{-SS_EPSILON, SS_EPSILON},
	{0.0, SS_EPSILON},
	{SS_EPSILON, SS_EPSILON},
};

static Object *findIntersection(RenderState& rs, Ray const& ray, float& dist, Vec& normal);
static Vec renderSample(RenderState& rs, Ray const& R, unsigned int depth);
static uint32_t mapRGB(Vec const& c);
static Ray rayThroughCameraPixel(RenderState const& rs, float px, float py);

static Ray rayThroughCameraPixel(RenderState const& rs, float px, float py) 
{
	Vec camdir((px / (float)rs.pixelWidth - 0.5f) * rs.camera.width, 
			   (py / (float)rs.pixelHeight - 0.5f) * rs.camera.height,
			   rs.camera.screenDist);
	return Ray(rs.camera.eye, rs.camera.viewM.apply(camdir));
}

static void showProgress(unsigned int line, unsigned int height)
{
	std::clog << '\r' << std::setw(3) << (line * 100) / height << "% - line " << line << " of " << height; 
}

static Vec renderSample(RenderState& rs, Ray const& R, unsigned int depth)
{
	if (depth >= rs.maxDepth)
    	return COLOR_BLACK;

	Vec N;
	float dist;
	Object *obj = findIntersection(rs, R, dist, N);

	if (obj == NULL) {
		return COLOR_BLACK;
	}

	Vec Lo = COLOR_BLACK;
	Vec Le = COLOR_BLACK;
	Vec Rp;
	Point P = R.O + dist * R.D;
	bool bounce = true;
	Material* mat = obj->material;

	if (mat == NULL) {
		//std::cerr << "Warning: object " << obj << " has no material\n";
		return obj->emittance;
	}

	N = N.normalized();

	// calculate outgoing ray
	switch (mat->reflectionType) 
	{
	case R_SPECULAR:
		Rp = specularRay(N, R.D, mat->specular, bounce);
		break;
	case R_REFRACTIVE:
		if (dot(R.D,N) < 0) {
			Rp = refractedRay(N, R.D, mat->refractiveIndex);
		}
		else {
			Rp = refractedRay(-1.0*N, R.D, 1/mat->refractiveIndex);
		}
		break;
	case R_REFLECTIVE:
		Rp = reflectedRay(N, R.D);
		break;
	default:
		// assume diffuse reflection
		// TODO cosine-weighted distribution
		Rp = uniformRandomRay(N);
		//std::clog << "RR\n";
		break;
	}

	// ray bounced
	if (bounce) {
		// Sample light from generated ray
		Vec Li = renderSample(rs, Ray(P,Rp), depth+1);
		if (mat->reflectionType == R_REFRACTIVE || 
			mat->reflectionType == R_REFLECTIVE) {
			// purely reflective or refractive material: do not compute BRDFs
			Lo = Li;
		} else {
			// material has a BRDF : evaluate it
			Lo = obj->emittance + obj->color * evalBRDF(*mat, -1.0 * R.D, Rp, N) * Li * dot(Rp, N);
			//std::clog << Lo << '\n';
			//if (dot(Rp, N) < 0 ) std::clog << dot(Rp, N) << '\n';
		}
	} else {
	// no incoming light
		Lo = obj->emittance;
	}

  return Lo;
}

static Object *findIntersection(RenderState& rs, Ray const& ray, float& dist, Vec& normal)
{
	static const float INFINITE = 1e30; 
	dist = INFINITE;
	float near, far;
	Object *nearest = NULL;
	Vec N;
	for (std::vector<Object*>::iterator obj = rs.scene.begin(); obj != rs.scene.end(); ++obj)
	{
		if ((*obj)->geometry == NULL) {
			std::clog << "Warning: object" << *obj << " has no geometry\n";
		} else {
			if ((*obj)->geometry->intersect(ray, (*obj)->position, near, N)) {
				// confirmed hit
				if (near < dist) {
					dist = near;
					nearest = *obj;
					normal = N;
				}
			}
		}
	}
	return nearest;
}

// 
static uint32_t mapRGB(Vec const& c)
{
	Vec cl = clamp(c, 0.f, 1.f);
	uint8_t r = cl.x()*255.f;
	uint8_t g = cl.y()*255.f;
	uint8_t b = cl.z()*255.f;
	return r | g << 8 | b << 16 | 0xFF << 24;
}

// main rendering function
void render(RenderState& rs)
{
	float sppf = static_cast<float>(rs.samplesPerPixel);
	float ssppf = static_cast<float>(NUM_SUPERSAMPLES);

	for (unsigned int i = 0; i < rs.pixelHeight; ++i) 
	{
		for (unsigned int j = 0; j < rs.pixelWidth; ++j) 
		{
			Vec accum = Vec(0,0,0);
			if (rs.supersampling) {
				for (unsigned int ss = 0; ss < NUM_SUPERSAMPLES; ++ss)
				{
					Vec sample;
					for (unsigned int s = 0; s < rs.samplesPerPixel; ++s) {
						sample = sample + renderSample(rs, rayThroughCameraPixel(rs, j+ssdisp[ss][0], i+ssdisp[ss][1]), 0);
					}
	        		sample = clamp(sample * (1 / sppf), 0.f, 1.f);
					accum = accum + sample;
					rs.numSamples++;
				}
				accum = accum * (1 / ssppf);
			} else {
				Vec sample;
				for (unsigned int s = 0; s < rs.samplesPerPixel; ++s) {
					sample = sample + renderSample(rs, rayThroughCameraPixel(rs, j, i), 0);
				}
				rs.numSamples++;
        		accum = sample * (1 / sppf);
			}
					
			rs.buffer[i*rs.pixelWidth+j] = mapRGB(accum);
		}
		rs.numRenderedLines++;
		showProgress(i+1, rs.pixelHeight);
	}
}
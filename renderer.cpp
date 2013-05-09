#include "renderer.hpp"
#include "util.hpp"
#include "sphere.hpp"
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
static Vec sample(RenderState& rs, 
				  Vec const& in,
				  Point const& hit, 
				  Vec const& normal, 
				  Object const* obj, 
				  unsigned int depth);
static Vec trace(RenderState& rs, Ray const& ray, unsigned int depth, bool seeLight);
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

// evaluate outgoing radiance at a given point
static Vec sample(RenderState& rs, 
				  Vec const& in,
				  Point const& hit, 
				  Vec const& normal, 
				  Object const* obj, 
				  unsigned int depth)
{
	Vec accum = Vec(0,0,0);
	Vec out, value, e;
	const float invsppf = 1.f / static_cast<float>(rs.samplesPerPixel);
	bool seeLight = true;
	float sampleX = frand(0,1);
	float sampleY = frand(0,1);

	BSDF const* bsdf = obj->getBSDF();
	Vec const& color = obj->getColor();
	
	if (bsdf->isSpecular()) {
		// perfect specular reflection, no contribution from other light sources
		value = bsdf->sample(in, normal, out, sampleX, sampleY);
		return color*value*trace(rs, Ray(hit,out), depth+1, seeLight);
		// Note: value should be (1,1,1) 
	} else {
		//if (depth = 0) {
		// TODO other samplers
		// sample at intersection point
		value = bsdf->sample(in, normal, out, sampleX, sampleY);
		seeLight = false;
		
		// calculate direct lighting
		for (std::vector<Object*>::iterator obj = rs.scene.begin(); obj != rs.scene.end(); ++obj)
		{
			if ((*obj)->getEmittance() == Vec(0,0,0)) {
				//std::cout << "OBJ\n";
				continue;
			}
			//std::cout << "LIGHT: ";
			// calculate solid angle
			// Position of sphere (assume it's a spherical light source)
			Vec L = (*obj)->getPosition();
			Vec D = (L - hit);
			float d2 = dot(D,D);
			// Radius?
			float r = static_cast<Sphere const*>((*obj)->getGeometry())->radius;
			// apparent angle
			float cos_max = sqrtf(d2 / (r*r + d2));
			// solid angle
			float omega = 2*M_PI*(1-cos_max);
			// generate a ray 
			Vec T,S;
			genOrtho(D.normalized(), T, S);

			float s1 = frand(0,1);	// TODO samplers
			float s2 = frand(0,1);

			float phi = 2*M_PI*s1;
			float v = 1.f - s2 * (1 - cos_max);
			float w = sqrt(1 - v*v);
			Vec Rd = cos(phi) * w * T + sin(phi) * w * S + v * D.normalized();
			Rd = Rd.normalized();

			if (dot(Rd, normal) < EPSILON) {
				// miss
				//std::cout << "MISS\n";
			}
			else {
				// shoot shadow ray
				float unused_dist;
				Vec unused_normal;
				Object *obj2 = findIntersection(rs, Ray(hit, Rd), unused_dist, unused_normal);
				if (obj2 == *obj) {
					e = e + (*obj)->getEmittance() * (bsdf->eval(in, Rd, normal) * omega * (1.f / M_PI) * dot(normal, Rd));
					//std::cout << "OK\n";
 
				} else {
					//std::cout << "SHADOW\n";
				}
			}
		}
		
		if (rs.directLighting) {
			// compute only direct lighting
			return color*e;
		}
		else {
			return color*(e + value*trace(rs, Ray(hit,out), depth+1, seeLight));
		}
	}
}

/*
 * Cast a ray and returns incoming radiance
 *
 */
static Vec trace(RenderState& rs, Ray const& ray, unsigned int depth, bool seeLight)
{
	/*
	 * Stop tracing when the maximum depth is reached
	 */
	if (depth >= rs.maxDepth)
    	return COLOR_BLACK;

	Vec normal;
	float dist;

	// find nearest intersecting object
	Object *obj = findIntersection(rs, ray, dist, normal);
	// no intersection : return ambient color
	if (obj == NULL) {
		return COLOR_BLACK;
	}
	normal = normal.normalized();

	// hit point
	Point hit = ray.O + dist * ray.D;
	// skip sampling if this is a light source
	if (obj->isLightSource()) {
		return seeLight ? obj->getEmittance() : Vec(0,0,0);
	}


	// Sample radiance at hit point
	return sample(rs, -1.f*ray.D, hit, normal, obj, depth);
}



/*static Vec tracePrimaryRay(RenderState& rs, Ray const& ray)
{
	Vec normal, accum;
	float dist;

	// find nearest intersecting object
	Object *obj = findIntersection(rs, ray, dist, normal);
	// no intersection : return ambient color
	if (obj == NULL) {
		return COLOR_BLACK;
	}
	normal = normal.normalized();

	// hit point
	Point hit = ray.O + dist * ray.D;
	// skip sampling if this is a light source
	if (obj->isLightSource()) {
		return obj->getEmittance();
	}

	// approximate radiance using monte-carlo method
	for (unsigned int i = 0; i < rs.samplesPerPixel; ++i) {
		accum = accum + sample(rs, ray.D, hit, normal, obj, 0);
	}
	const float invsppf = 1.f / static_cast<float>(rs.samplesPerPixel);
	return accum * invsppf;
}*/


static Object *findIntersection(RenderState& rs, Ray const& ray, float& dist, Vec& normal)
{
	static const float INFINITE = 1e30; 
	dist = INFINITE;
	float near, far;
	Object *nearest = NULL;
	Vec N;
	for (std::vector<Object*>::iterator obj = rs.scene.begin(); obj != rs.scene.end(); ++obj)
	{
		Geometry const* geometry = (*obj)->getGeometry();
		if (geometry == NULL) {
			std::clog << "Warning: object" << *obj << " has no geometry\n";
		} else {
			if (geometry->intersect(ray, (*obj)->getPosition(), near, N)) {
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

static Vec samplePixel(RenderState& rs, float x, float y)
{
	const float invsppf = 1.f / static_cast<float>(rs.samplesPerPixel);
	Vec sample;
	// approximate radiance using monte-carlo method
	for (unsigned int i = 0; i < rs.samplesPerPixel; ++i) {
		float xrand = frand(-0.5, 0.5);
		float yrand = frand(-0.5, 0.5);
		sample = sample + trace(rs, rayThroughCameraPixel(rs, x + xrand , y + yrand), 0, true);
	}
	sample = sample * invsppf;
	sample = clamp(sample, 0.f, 1.f);
	return sample;
}

// main rendering function
void render(RenderState& rs)
{
	//float sppf = static_cast<float>(rs.samplesPerPixel);
	const float invssppf = 1.f / static_cast<float>(NUM_SUPERSAMPLES);

#pragma omp parallel for
	for (int i = 0; i < rs.pixelHeight; ++i) 
	{
		for (int j = 0; j < rs.pixelWidth; ++j) 
		{
			Vec accum = Vec(0,0,0);
			if (rs.supersampling) {
				for (unsigned int ss = 0; ss < NUM_SUPERSAMPLES; ++ss) {
					accum = accum + samplePixel(rs, j+ssdisp[ss][0], i+ssdisp[ss][1]);
					rs.numSamples++;
				}
				accum = accum * invssppf;
			} else {
				accum = samplePixel(rs, j, i);
				rs.numSamples++;
			}
					
			rs.buffer[i*rs.pixelWidth+j] = mapRGB(accum);
		}
		rs.numRenderedLines++;
		showProgress(i+1, rs.pixelHeight);
	}
}
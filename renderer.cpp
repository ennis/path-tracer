#include "renderer.hpp"
#include "util.hpp"
#include "sphere.hpp"
#include "primitive.hpp"
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

static bool findIntersection(std::vector<Primitive*> const& scene, Ray const& ray, Intersection *isect, Material const** mat);

static uint32_t mapRGB(Vec const& c);

static void showProgress(unsigned int line, unsigned int height)
{
	std::clog << '\r' << std::setw(3) << (line * 100) / height << "% - line " << line << " of " << height; 
}

Vec Renderer::evaluateDirectLighting(Intersection const& isect, Material const* mat)
{
	for (std::vector<Primitive*>::const_iterator p = m_scene->begin(); p != m_scene->end(); ++p)
	{
		Material const *light_mat = (*p)->getMaterial();
		if (light_mat->getEmittance() == Vec(0,0,0)) {
			//std::cout << "OBJ\n";
			continue;
		}
		//std::cout << "LIGHT: ";
		// calculate solid angle
		// Position of sphere (assume it's a spherical light source)
		GeometricPrimitive const *geom = static_cast<GeometricPrimitive const*>(*p);
		Sphere const* sphere = static_cast<Sphere const*>(geom->getGeometry());
		Vec L = sphere->getCenter();
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
}

// evaluate outgoing radiance at a given point
Vec Renderer::trace(Ray const& ray, unsigned int depth, bool seeLight = true)
{
	//
	// Stop tracing when the maximum depth is reached
	//
	if (depth >= m_options.maxDepth) {
    	return m_ambient;
	}

	Intersection isect;
	Material const *mat;

	//
	// find nearest intersecting object
	//
	if (!findIntersection(*m_scene, ray, &isect, &mat)) {
		// no intersection : return ambient color
		// TODO environment maps
		return m_ambient;
	}

	//
	// skip sampling if this is a light source
	//
	if (mat->isLightSource()) {
		return seeLight ? mat->getEmittance() : Vec(0,0,0);
	}


	Vec accum = Vec(0,0,0);
	Vec out, value, e;
	const float invsppf = 1.f / static_cast<float>(m_options.samplesPerPixel);
	bool nextSeeLight = true;
	float sampleX = frand(0,1);
	float sampleY = frand(0,1);
	bool specular;

	//
	// Get the BSDF for the sampled material
	//
	BSDF const* bsdf = mat->getBSDF();
	
	//
	// Sample texture at intersection point
	//
	Vec color = mat->getTexture()->sample(isect.u, isect.v);

	//
	// Sample the BSDF at the intersection point
	//
	value = bsdf->sample(isect.N, ray.D, color, sampleX, sampleY, out, specular);
	
	if (specular) {	
		//
		// Perfect specular reflection : do not evaluate the contribution of light sources 
		//
		return color*value*trace(Ray(isect.P, out), depth+1, nextSeeLight);
		// Note: value should be (1,1,1) 
	} else {
		//
		// evaluate direct lighting
		if (rs.explicitLightSampling) {
			seeLight = false;
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
		}
		
		if (rs.directLightingOnly) {
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


static bool findIntersection(std::vector<Primitive*> const& scene, Ray const& ray, Intersection *isect, Material const** mat)
{
	static const float INFINITE = 1e30; 
	float dist = INFINITE;
	float near, far;
	Intersection current_isect;
	bool hit = false;
	for (std::vector<Primitive*>::iterator p = scene.begin(); p != scene.end(); ++p)
	{
		if ((*p)->intersect(ray, &current_isect, mat)) {
			// confirmed hit
			if (hit == false || isect->dist < current_isect.dist) {
				hit = true;
				isect = current_isect;
			}
		}
	}
	return hit;
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
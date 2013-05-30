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

static void showProgress(unsigned int line, unsigned int height)
{
	std::clog << '\r' << std::setw(3) << (line * 100) / height << "% - line " << line << " of " << height; 
}

Vec Renderer::evaluateDirectLighting(Intersection const& isect, Vec const& in, Vec const& color, BSDF const* bsdf)
{
	Vec e = nullVec;
	for (std::vector<Primitive const*>::const_iterator p = m_scene->cbegin(); p != m_scene->cend(); ++p)
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
		Point L = sphere->getCenter();
		Vec D = (L - isect.P);
		float d2 = dot(D,D);
		// Radius?
		float r = sphere->getRadius();
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

		if (dot(Rd, isect.N) < EPSILON) {
			// miss
			//std::cout << "MISS\n";
		}
		else {
			// shoot shadow ray
			Intersection isect2;
			Material const* tmp_mat;
			findIntersection(*m_scene, Ray(isect.P, Rd), &isect2, &tmp_mat);
			if (isect2.geometry == sphere) {
				e += light_mat->getEmittance() * bsdf->eval(isect.N, in, Rd, color) * omega * (1.f / M_PI) * dot(isect.N, Rd);
				//std::cout << "OK\n";
			} else {
				//std::cout << "SHADOW\n";
			}
		}
	}
	return e;
}

// evaluate outgoing radiance at a given point
Vec Renderer::trace(Ray const& ray, unsigned int depth, bool seeLight)
{
	//
	// Stop tracing when the maximum depth is reached
	//
	if (depth >= m_params.maxDepth) {
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

	//return Vec(1.f,1.f,1.f) * isect.dist / 20.f;

	//
	// skip sampling if this is a light source
	//
	if (mat->isLightSource()) {
		return seeLight ? mat->getEmittance() : Vec(0,0,0);
	}


	Vec out, value, e;
	const float invsppf = 1.f / static_cast<float>(m_params.samplesPerPixel);
	bool nextSeeLight = true;
	float sampleX = frand(0,1);
	float sampleY = frand(0,1);

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
	bool specular;
	value = bsdf->sample(isect.N, -ray.D, color, sampleX, sampleY, out, specular);
	
	if (specular) {	
		//
		// Perfect specular reflection : do not evaluate the contribution of light sources 
		//
		return color*value*trace(Ray(isect.P, out), depth+1, nextSeeLight);
		// Note: value should be (1,1,1) 

	} else {

		//
		// evaluate direct lighting
		if (m_params.explicitLightSampling) {
			nextSeeLight = false;
			e = evaluateDirectLighting(isect, -ray.D, color, bsdf);
		}
		
		if (m_params.directLightingOnly) {
			// compute only direct lighting
			return color*e;
		}
		else {
			return color*(e + value*trace(Ray(isect.P, out), depth+1, nextSeeLight));
		}
	}
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


bool findIntersection(std::vector<Primitive const*> const& scene, Ray const& ray, Intersection *isect, Material const** mat)
{
	static const float INFINITE = 1e30f;
	Intersection current_isect;
	Material const *tmp_mat;
	isect->dist = INFINITE;
	bool hit = false;
	for (std::vector<Primitive const*>::const_iterator p = scene.cbegin(); p != scene.cend(); ++p)
	{
		if ((*p)->intersect(ray, &current_isect, &tmp_mat)) {
			// confirmed hit
			if (hit == false || current_isect.dist < isect->dist) {
				hit = true;
				*isect = current_isect;
				*mat = tmp_mat;
			}
		}
	}
	return hit;
}

// 
uint32_t mapRGB(Vec const& c)
{
	Vec cl = clamp(c, 0.f, 1.f);
	uint8_t r = static_cast<uint8_t>(cl.x()*255.f);
	uint8_t g = static_cast<uint8_t>(cl.y()*255.f);
	uint8_t b = static_cast<uint8_t>(cl.z()*255.f);
	return r | g << 8 | b << 16 | 0xFF << 24;
}

Vec Renderer::samplePixel(float x, float y)
{
	const float invsppf = 1.f / static_cast<float>(m_params.samplesPerPixel);
	Vec sample;
	for (unsigned int i = 0; i < m_params.samplesPerPixel; ++i) {
		float xrand = frand(-0.5, 0.5);
		float yrand = frand(-0.5, 0.5);
		sample = sample + trace(
					m_camera->rayThroughCameraPixel(
						x + xrand, 
						y + yrand, 
						static_cast<float>(m_params.pixelWidth),
						static_cast<float>(m_params.pixelHeight)), 
					0, 
					true);
	}
	sample = sample * invsppf;
	sample = clamp(sample, 0.f, 1.f);
	return sample;
}

Vec Renderer::samplePixelProgressive(float x, float y)
{
	float xrand = frand(-0.5, 0.5);
	float yrand = frand(-0.5, 0.5);
	return trace(m_camera->rayThroughCameraPixel(x + xrand , y + yrand, m_params.pixelWidth, m_params.pixelHeight), 0, true);
}

void Renderer::render(RenderParameters const& params, Camera const& camera, std::vector<Primitive const*> const& scene, Film& out_film)
{
	m_params = params;
	m_scene = &scene;
	m_camera = &camera;

	m_started = true;

	if (params.progressive) {
		renderProgressive(out_film);
	} else {
		renderScanline(out_film);
	}
}

void Renderer::renderScanline(Film& out_film)
{
	//float sppf = static_cast<float>(rs.samplesPerPixel);
	const float invssppf = 1.f / static_cast<float>(NUM_SUPERSAMPLES);

#pragma omp parallel for
	for (int i = 0; i < m_params.pixelHeight; ++i) 
	{
		for (int j = 0; j < m_params.pixelWidth; ++j) 
		{
			Vec accum = Vec(0,0,0);
			if (m_params.supersampling) {
				for (unsigned int ss = 0; ss < NUM_SUPERSAMPLES; ++ss) {
					accum = accum + samplePixel(j+ssdisp[ss][0], i+ssdisp[ss][1]);
					m_samples++;
				}
				accum = accum * invssppf;
			} else {
				accum = samplePixel(static_cast<float>(j), static_cast<float>(i));
				m_samples++;
			}
					
			out_film.accumPixel(j, i, accum);
		}
		m_lines++;
		showProgress(m_lines, m_params.pixelHeight);
	}

	m_finished = false;
}

void Renderer::renderProgressive(Film& out_film)
{
	while (!m_finished) 
	{
		#pragma omp parallel for
		for (int i = 0; i < m_params.pixelHeight; ++i) 
		{
			for (int j = 0; j < m_params.pixelWidth; ++j) 
			{
				Vec accum = samplePixelProgressive(static_cast<float>(j), static_cast<float>(i));
				out_film.accumPixel(j, i, accum);
				m_samples++;
			}
		}
		m_frames++;
	}
}
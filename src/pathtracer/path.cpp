#include <iostream>
#include <cassert>
#include <iomanip>

#include "path.hpp"
#include "util.hpp"
#include "primitive.hpp"
#include "bsdf.hpp"

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

/*static void showProgress(unsigned int line, unsigned int height)
{
	std::clog << '\r' << std::setw(3) << (line * 100) / height << "% - line " << line << " of " << height; 
}*/

/*static inline Vec directIllum(Intersection const& isect, Primitive const &light, Vec &WiW, float &pdf)
{
	Intersection lightSample;
	light.sample(isect.P, lightSample, pdf);
	// TODO texture
	WiW = (lightSample.P - isect.P).normalized();
	return light.getEmittance();
}*/


static bool isOccluded(Scene const &scene, Point const &P, Vec const &dir, float maxt)
{
	Intersection test;
	return scene.findIntersection(Ray(P, dir), &test, maxt);
}


Vec PathRenderer::evaluateDirectLighting(Intersection const &isect)
{
	Vec Ld, WiW;
	float pdf, maxt;
	float u1, u2;
	for (auto p = m_scene->getEmitters().cbegin(); 
		p != m_scene->getEmitters().cend();
		++p) 
	{
		for (int i = 0; i < m_params->numShadowRays; ++i) {
			m_lightSampler->getNext2D(u1, u2);
			Vec E = (*p)->sampleLight(isect.P, u1, u2, WiW, pdf, maxt);
			// check if ray is occluded
			if (isOccluded(*m_scene, isect.P, WiW, maxt)) {
				//std::clog << "occluded\n";
				continue;
				//if (distance(test.P, isect.P) > EPSILON) {
					//std::clog << "nohit\n";
					//continue;
				//}
				//if (test.primitive == isect.primitive) {
					//std::clog << "self-intersection\n";
					//__debugbreak();
				//}
			} 
			//Vec WiL = isect.toLocal(R.D);
			// Geometry term
			Ld += E * isect.primitive->getMaterial()->eval(isect, WiW) * dot(WiW, isect.N) / pdf;
		}
		Ld *= 1.f / static_cast<float>(m_params->numShadowRays);
	}
	return Ld;
}

/*Vec PathRenderer::evaluateDirectLighting(Intersection const& isect, Vec const& in, Vec const& color, BSDF const* bsdf)
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
}*/

// for each area light
//		sample
//		

//===================================================
// PathRenderer::trace (private)
// evaluate outgoing radiance at a given point
Vec PathRenderer::trace(Ray const& R, unsigned int depth, bool seeLight, bool debug)
{
	// Stop tracing when the maximum depth is reached
	/*if (depth >= m_params->maxDepth) {
		return Vec(0.f,0.f,0.f);
	}*/


	Intersection isect;
	Vec Li, E, Ambient;
	Ray Rp = R;
	Vec C = Vec(1.f, 1.f, 1.f);
	float u1, u2;

	for (unsigned int i = 0; i < 5; ++i) {
	
		//===========================================
		// find intersection
		if (!m_scene->findIntersection(Rp, &isect)) {
			// no intersection, add contribution of ambient light
			// and terminate path
			Li += C * m_scene->evalAmbient(Rp.D);
			break;
		}

		//===========================================
		// add emissive component and continue path
		E = isect.primitive->getEmittance();

		// skip sampling if the object is emissive
		if (!(E == nullVec)) {
			Li += C * (seeLight ? E : Vec(0.f,0.f,0.f));
			break;
		}

		//===========================================
		// Material sampling
		float pdfResult;
		Vec WiW;
		int bxdfType;
		Material const *material = isect.primitive->getMaterial();
		material->init(isect);

		//===========================================
		// Next path
		m_bxdfSampler->getNext2D(u1, u2);
		Vec Ci = material->sample(isect, u1, u2, WiW, pdfResult, bxdfType);

		//===========================================
		// Russian roulette termination
		if (depth >= 3) {
			float pterm = std::max(C.x(), std::max(C.y(), C.z()));
			if (frand(0.f, 1.f) > pterm) {
				// terminate
				break;
			} else {
				C *= 1.f / pterm;
			}
		}

		//===========================================
		// Direct lighting
		if (m_params->directLightingOnly) {
			if (!(bxdfType & BxDF_SPECULAR)) {
				Li += C * evaluateDirectLighting(isect);
			}
			break;
		}

		seeLight = true;
		if (!(bxdfType & BxDF_SPECULAR) && m_params->explicitLightSampling) {
			Li += C * evaluateDirectLighting(isect);
			seeLight = false;
		}

		C *= Ci;
		
		Rp.O = isect.P;
		Rp.D = WiW;
	}

	return Li;
}

//===================================================
// PathRenderer::samplePixel
Vec PathRenderer::samplePixel(float x, float y, bool debug)
{
	const float invsppf = 1.f / static_cast<float>(m_params->samplesPerPixel);
	Vec sample;
	float xrand;
	float yrand;
	for (unsigned int i = 0; i < m_params->samplesPerPixel; ++i) {
		m_subsampleSampler->getNext2D(xrand, yrand);
		xrand -= 0.5f;
		yrand -= 0.5f;
		sample = sample + trace(
			m_scene->getCamera()->rayThroughCameraPixel(
				x + xrand, 
				y + yrand, 
				static_cast<float>(m_params->pixelWidth),
				static_cast<float>(m_params->pixelHeight)), 
			0, 
			true,
			debug);
	}
	sample = sample * invsppf;
	sample = clamp(sample, 0.f, 1.f);
	return sample;
}

//===================================================
// PathRenderer::samplePixelProgressive
Vec PathRenderer::samplePixelProgressive(int px, int py, float &x, float &y, bool debug)
{
	float xrand;
	float yrand;
	m_subsampleSampler->getNext2D(xrand, yrand);
	x = static_cast<float>(px) + xrand - 0.5f;
	y = static_cast<float>(py) + yrand - 0.5f;

	return trace(
		m_scene->getCamera()->rayThroughCameraPixel(
			x, y, 
			m_params->pixelWidth,
			m_params->pixelHeight), 
		0,
		true,
		debug);
}

//===================================================
// PathRenderer::render
void PathRenderer::render(
	Scene const& scene,
	Film& film,
	RenderParameters const& renderParameters)
{
	m_params = &renderParameters;
	m_scene = &scene;
	m_film = &film;
	m_started = true;
	if (m_params->progressive) {
		renderProgressive();
	} else {
		renderScanline();
	}
}

//===================================================
// PathRenderer::renderScanline
void PathRenderer::renderScanline()
{
	//float sppf = static_cast<float>(rs.samplesPerPixel);
	const float invssppf = 1.f / static_cast<float>(NUM_SUPERSAMPLES);

#pragma omp parallel for
	for (int i = 0; i < m_params->pixelHeight; ++i) 
	{
		for (int j = 0; j < m_params->pixelWidth; ++j) 
		{
			bool debug = false;
			if (j == m_params->debugPixelX && i == m_params->debugPixelY) {
				debug = true;
			}
			Vec accum = Vec(0,0,0);
			if (m_params->supersampling) {
				for (unsigned int ss = 0; ss < NUM_SUPERSAMPLES; ++ss) {
					accum = accum + samplePixel(
						j+ssdisp[ss][0], 
						i+ssdisp[ss][1],
						debug);
					m_samples++;
				}
				accum = accum * invssppf;
			} else {
				accum = samplePixel(
					static_cast<float>(j), 
					static_cast<float>(i),
					debug);
				m_samples++;
			}
					
			m_film->addSample(j, i, accum);
		}
		m_lines++;
	}

	m_finished = false;
}

void PathRenderer::renderProgressive()
{
	while (!m_finished) 
	{
		#pragma omp parallel for
		for (int i = 0; i < m_params->pixelHeight; ++i) 
		{
			for (int j = 0; j < m_params->pixelWidth; ++j) 
			{
				bool debug = false;
				if (j == m_params->debugPixelX && i == m_params->debugPixelY) {
					debug = true;
				}
				float x, y;
				Vec accum = samplePixelProgressive(j, i, x, y, debug);
				m_film->addSample(x, y, accum);
				m_samples++;
			}
		}
		m_frames++;
	}
}
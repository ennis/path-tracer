#include <iostream>
#include <cassert>
#include <iomanip>

#include "path.hpp"
#include "util.hpp"
#include "primitive.hpp"

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

static inline Vec directIllum(Intersection const& isect, Primitive const &light, Vec &WiW, float &pdf)
{
	Intersection lightSample;
	light.sample(isect.P, lightSample, pdf);
	// TODO texture
	WiW = (lightSample.P - isect.P).normalized();
	return light.getEmittance();
}

Vec PathRenderer::evaluateDirectLighting(Intersection const& isect, Vec const &WoL, Vec const &textureColor)
{
	Vec Ld, Wi;
	float pdf, lightPdf;
	Ray R;
	R.O = isect.P;
	Intersection occluded;
	Intersection lightSample;
	for (auto p = m_scene->getAreaLights().cbegin(); 
		p != m_scene->getAreaLights().cend();
		++p) 
	{
		Primitive const *light = *p;
		light->sample(isect.P, lightSample, lightPdf);
		Vec XX = lightSample.P - isect.P;
		R.D = XX.normalized();

		// check if ray is occluded
		//std::clog << lightSample.P << '\n';
		bool hit = m_scene->findIntersection(R, &occluded);
		if (!hit || occluded.primitive != light) {
			// occluded
			//std::clog << "SHADOW " << hit << "\n";
		} else {
			//std::clog << "OK\n";
			Vec bxdfValue;
			Vec WiL = isect.worldToLocal(R.D);
			isect.primitive->getBxDF()->eval(textureColor, WoL, WiL, bxdfValue, pdf);
			
			//std::clog << Ei << '\n';
			Vec Ldi = light->getEmittance();
			// Geometry term
			//std::clog << lightPdf << " " << dot(XX, XX) << '\n';
			Ldi *= dot(R.D, isect.N) * dot(lightSample.N, -R.D) / (dot(XX, XX) * lightPdf);
			Ld += Ldi * bxdfValue;
			//std::clog << Ld << '\n';
		}
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
// isect -> texture sampling  ..........-> blend -> RETURN
//       -> material sampling -> trace..

Vec PathRenderer::trace(Ray const& R, unsigned int depth, bool seeLight)
{
	// Stop tracing when the maximum depth is reached
	if (depth >= m_params->maxDepth) {
		return Vec(0.f,0.f,0.f);
	}

	Intersection isect;
	EnvironmentMap const *envmap = m_scene->getEnvironmentMap();
	
	//===========================================
	// find intersection
	if (!m_scene->findIntersection(R, &isect)) {
		// no intersection : return ambient color
		// TODO environment maps
		if (envmap) {
			return envmap->eval(R.D);
		} else {
			return m_scene->getAmbient();
		}
	}

	//if (depth > 0) {
	//	std::clog << "Self intersection\n";
	//}

	Vec E = isect.primitive->getEmittance();

	//
	// skip sampling if this is a light source
	//
	if (!(E == nullVec)) {
		return seeLight ? E : Vec(0.f,0.f,0.f);
	}

	
	//===========================================
	// Texture sampling
	Texture const *texture = isect.primitive->getTexture();
	Vec textureColor;
	if (texture != NULL) {
		textureColor = texture->sample(isect.u, isect.v);
	} else {
		// TODO missing texture
		textureColor = Vec(0.f, 1.f, 0.f);
	}

	//===========================================
	// world to local
	Vec WoL = isect.worldToLocal(-R.D);

	//===========================================
	// BSDF sampling
	Vec bxdfResult;
	float pdfResult;
	Vec WiL;
	int bxdfType;
	BxDF const *bxdf = isect.primitive->getBxDF();

	if (bxdf != NULL) {
		bxdf->sample(textureColor, WoL, WiL, bxdfResult, pdfResult, bxdfType);
	} else {
		// no BSDF, return texture color
		return E + textureColor;
	}

	//===========================================
	// Transform output direction to world space
	Vec Wi = isect.localToWorld(WiL); 
	//std::clog << "WoL: " << WoL << "\nWo: " << Wo << "\n";

	//===========================================
	// Continue path
	bool nextSeeLight = true;
	if (bxdfType & BxDF_SPECULAR) {	
		//
		// Perfect specular reflection : do not evaluate the contribution of light sources 
		// 
		E += bxdfResult*trace(Ray(isect.P, Wi), depth + 1, nextSeeLight);
	} else {
		nextSeeLight = false;
		E += evaluateDirectLighting(isect, WoL, textureColor);
		if (!m_params->directLightingOnly) {
			//E += bxdfResult*trace(Ray(isect.P, Wi), depth + 1, nextSeeLight);
		}
	}
		// Note: value should be (1,1,1) 
	//} else {		
	//	return E + textureColor*bxdfResult*trace(Ray(isect.P, Wo), depth+1, nextSeeLight);
	//}
	return E;
}

//===================================================
// PathRenderer::samplePixel
Vec PathRenderer::samplePixel(float x, float y)
{
	const float invsppf = 1.f / static_cast<float>(m_params->samplesPerPixel);
	Vec sample;
	for (unsigned int i = 0; i < m_params->samplesPerPixel; ++i) {
		float xrand = frand(-0.5, 0.5);
		float yrand = frand(-0.5, 0.5);
		sample = sample + trace(
			m_scene->getCamera()->rayThroughCameraPixel(
				x + xrand, 
				y + yrand, 
				static_cast<float>(m_params->pixelWidth),
				static_cast<float>(m_params->pixelHeight)), 
			0, 
			true);
	}
	sample = sample * invsppf;
	sample = clamp(sample, 0.f, 1.f);
	return sample;
}

//===================================================
// PathRenderer::samplePixelProgressive
Vec PathRenderer::samplePixelProgressive(float x, float y)
{
	float xrand = frand(-0.5, 0.5);
	float yrand = frand(-0.5, 0.5);
	return trace(
		m_scene->getCamera()->rayThroughCameraPixel(
			x + xrand, 
			y + yrand, 
			m_params->pixelWidth,
			m_params->pixelHeight), 
		0,
		true);
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
			Vec accum = Vec(0,0,0);
			if (m_params->supersampling) {
				for (unsigned int ss = 0; ss < NUM_SUPERSAMPLES; ++ss) {
					accum = accum + samplePixel(
						j+ssdisp[ss][0], 
						i+ssdisp[ss][1]);
					m_samples++;
				}
				accum = accum * invssppf;
			} else {
				accum = samplePixel(
					static_cast<float>(j), 
					static_cast<float>(i));
				m_samples++;
			}
					
			m_film->accumPixel(j, i, accum);
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
				Vec accum = samplePixelProgressive(
					static_cast<float>(j), 
					static_cast<float>(i));
				m_film->accumPixel(j, i, accum);
				m_samples++;
			}
		}
		m_frames++;
	}
}
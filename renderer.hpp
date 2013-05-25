#pragma once
#include <vector>
#include "camera.hpp"
#include "vec.hpp"
#include "ray.hpp"
#include "primitive.hpp"
#include <cstdint>

static const unsigned int DEFAULT_MAX_DEPTH = 5;
static const unsigned int DEFAULT_SPP = 10;


struct RenderOptions
{
	// parameters
	unsigned int samplesPerPixel;
	unsigned int maxDepth;
	unsigned int pixelWidth;
	unsigned int pixelHeight;
	uint32_t *buffer;

	bool supersampling;
	bool cosineWeightedSampling;
	bool directLightingOnly;
	bool explicitLightSampling;
	
	unsigned int numRenderedLines;
	unsigned int numSamples;
};

class Film
{
public:
	Film(unsigned int pixelWidth, unsigned int pixelHeight) : m_pixelWidth(pixelWidth), m_pixelHeight(pixelHeight)
	{
		m_buffer = new uint32_t[pixelWidth * pixelHeight];
	}

	~Film()
	{
		delete [] m_buffer;
	}

	unsigned int getWidth() const {
		return m_pixelWidth;
	}

	unsigned int getHeight() const {
		return m_pixelHeight;
	}

	uint32_t const * getBuffer() const {
		return m_buffer;
	}

	void setPixel(unsigned int x, unsigned int y, Vec const& color) {
		if (x >= m_pixelWidth || y >= m_pixelHeight) {
			std::cerr << "Film::setPixel : invalid coordinates (" << x << "," << y << ")\n";
			return;
		}
		m_buffer[y*m_pixelWidth + x] = mapRGB(color);
	}

private:
	uint32_t *m_buffer;
	unsigned int m_pixelWidth, m_pixelHeight;
};

class Renderer
{
public:
	Renderer(RenderOptions const& options);

	void render(Camera const& camera, std::vector<Primitive*> const& scene);



private:

	// private methods

	Vec evaluateDirectLighting(Intersection const& isect, Material const* mat);
	Vec samplePixel(float x, float y);
	Vec trace(Ray const& ray, unsigned int depth, bool seeLight = true);

	RenderOptions m_options;
	Camera const * m_camera;
	Vec m_ambient;
	std::vector<Primitive*> const * m_scene;
	std::vector<Primitive*> const * m_lightSources;

};

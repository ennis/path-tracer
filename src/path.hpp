#ifndef PATH_HPP
#define PATH_HPP
#include <vector>
#include <cstdint>
#include <iostream>

#include "camera.hpp"
#include "vec.hpp"
#include "ray.hpp"
#include "primitive.hpp"
#include "scene.hpp"

static const unsigned int DEFAULT_MAX_DEPTH = 5;
static const unsigned int DEFAULT_SPP = 10;


struct RenderParameters
{
	// parameters
	unsigned int samplesPerPixel;
	unsigned int maxDepth;
	unsigned int pixelWidth;
	unsigned int pixelHeight;

	bool supersampling;
	bool cosineWeightedSampling;
	bool directLightingOnly;
	bool explicitLightSampling;
	bool progressive;
};


struct ProgressivePixel
{
	Vec value;
	unsigned int numSamples;
};

class Film
{
public:
	Film(unsigned int pixelWidth, unsigned int pixelHeight) : 
		m_pixelWidth(pixelWidth), 
		m_pixelHeight(pixelHeight)
	{
		m_buffer = new ProgressivePixel[pixelWidth * pixelHeight];
		for (unsigned int i = 0; i < pixelWidth * pixelHeight; ++i) {
			m_buffer[i].numSamples = 0;
		}
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

	ProgressivePixel const * getBuffer() const {
		return m_buffer;
	}

	void accumPixel(unsigned int x, unsigned int y, Vec const& color) {
		if (x >= m_pixelWidth || y >= m_pixelHeight) {
			std::cerr << "Film::setPixel : invalid coordinates (" << x << "," << y << ")\n";
			return;
		}
		m_buffer[y*m_pixelWidth + x].value += color;
		m_buffer[y*m_pixelWidth + x].numSamples++;
	}

	void convertToRGB(uint32_t * out_image) const {
		for (unsigned int x = 0; x < m_pixelWidth; ++x) {
			for (unsigned int y = 0; y < m_pixelHeight; ++y) {
				if (m_buffer[x + y*m_pixelWidth].numSamples != 0) {
					out_image[x + y*m_pixelWidth] = mapRGB(
						m_buffer[x + y*m_pixelWidth].value / 
							static_cast<float>(
								m_buffer[x + y*m_pixelWidth].numSamples));
				} else {
					out_image[x + y*m_pixelWidth] = mapRGB(Vec());
				}
			}
		}
	}

private:
	ProgressivePixel *m_buffer;
	unsigned int m_pixelWidth, m_pixelHeight;
};

class PathRenderer
{
public:
	PathRenderer() : 
		m_params(), m_scene(NULL), m_film(NULL),
		m_samples(0), m_frames(0), m_lines(0), 
		m_started(false), m_finished(false)
	{}

	void render(Scene const& scene,
		Film& film,
		RenderParameters const& renderParameters);
	
	bool isStarted() const {
		return m_started;
	}

	bool isFinished() const {
		return m_finished;
	}

	unsigned int getNumSamples() const {
		return m_samples;
	}

	unsigned int getNumFrames() const {
		return m_frames;
	}

	void stop() {
		m_finished = true;
	}

private:

	// private methods
	void renderProgressive();
	void renderScanline();
	Vec samplePixel(float x, float y);
	Vec samplePixelProgressive(float x, float y);
	Vec trace(Ray const& ray, unsigned int depth, bool seeLight = true);
	Vec evaluateDirectLighting(Intersection const& isect, Vec const &WoL, Vec const &textureColor);
	//Vec evaluateDirectLighting(Intersection const& isect, Vec const& in, Vec const& color, BSDF const* bsdf);

	RenderParameters const *m_params;
	Scene const *m_scene;
	Film *m_film;

	unsigned int m_samples;
	unsigned int m_frames;
	unsigned int m_lines;
	bool m_started;
	bool m_finished;
};

#endif
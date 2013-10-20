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
#include "film.hpp"

static const unsigned int DEFAULT_MAX_DEPTH = 5;
static const unsigned int DEFAULT_SPP = 10;


struct RenderParameters
{
	// parameters
	unsigned int samplesPerPixel;
	unsigned int maxDepth;
	unsigned int pixelWidth;
	unsigned int pixelHeight;

	int numShadowRays;

	bool supersampling;
	bool cosineWeightedSampling;
	bool directLightingOnly;
	bool explicitLightSampling;
	bool progressive;

	// debug
	int debugPixelX;
	int debugPixelY;
};



class Sampler
{
public:
	virtual float getNext() = 0;

	virtual void getNext2D(float &x, float &y) {
		x = getNext();
		y = getNext();
	}

	virtual int getNextInt(int min, int max) {
		return min + static_cast<int>(getNext() * (max - min + 1));
	}

	virtual void getNextInt2D(int minx, int maxx, int miny, int maxy, int &x, int &y) {
		float fx, fy;
		getNext2D(fx, fy);
		x = minx + static_cast<int>(fx * (maxx - minx + 1));
		y = miny + static_cast<int>(fy * (maxy - miny + 1));
	}
};

class SimpleSampler : public Sampler
{
public:
	virtual float getNext() {
		return frand(0.f, 1.f);
	}
private:
};

class Stratified1DSampler : public Sampler
{
public:
	Stratified1DSampler(int numStrata) : m_numStrata(numStrata), m_curStratum(0) {
		m_stratumSize = 1.f / static_cast<float>(numStrata);
		m_permutation = new int[numStrata];
		generatePermutation();
	}

	~Stratified1DSampler() {
		delete [] m_permutation;
	}

	void generatePermutation() 
	{
		for (int i = 0; i < m_numStrata; ++i) {
			int j = rand() % (i + 1);
			m_permutation[i] = m_permutation[j];
			m_permutation[j] = i;
		}
		for (int i = 0; i < m_numStrata; ++i) {
			//std::cout << m_permutation[i] << '\n';
		}
	}

	float getInStratum() const {
		return m_permutation[m_curStratum] * m_stratumSize + frand(0.f, m_stratumSize);
	}

	virtual float getNext() {
		float value = getInStratum();
		nextStratum();
		if (value < 0.f || value > 1.f) {
			std::cout << value << '\n';
		}
		return value;
	}

	void nextStratum() {
		++m_curStratum;
		if (m_curStratum >= m_numStrata) {
			generatePermutation();
			m_curStratum = 0;
		}
	}

	int getCurrentStratum() const {
		return m_curStratum;
	}

	/*virtual float getNext2D() {
		float value = m_curStratum * m_stratumSize + frand(0.f, m_stratumSize);
		m_curStratum = (m_curStratum + 1) % m_numStrata;
		return value;
	}*/


private:
	int m_numStrata;
	int m_curStratum;
	float m_stratumSize;
	int *m_permutation;
};

class Stratified2DSampler : public Sampler
{
public:
	Stratified2DSampler(int numStrataX, int numStrataY) : m_samplerX(numStrataX), m_samplerY(numStrataY) 
	{
	}

	~Stratified2DSampler() {
	}


	virtual float getNext() {
		return m_samplerX.getNext();
	}

	virtual void getNext2D(float &x, float &y) {
		y = m_samplerY.getInStratum();
		x = m_samplerX.getNext();
		if (m_samplerX.getCurrentStratum() == 0) {
			m_samplerY.nextStratum();
		}
	}


private:
	Stratified1DSampler m_samplerX, m_samplerY;
};

class Halton1DSampler : public Sampler {
public:
	Halton1DSampler(int base) {
		m_invBase = 1.f / static_cast<float>(base);
		m_value = 0.f;
	}

	/*void number(int i,int base) {
		double f = m_invBase = 1.0/base;
		value = 0.0;
		while(i>0) {
			value += f*(double)(i%base);
			i /= base;
			f *= inv_base;
		}
	}*/

	float getNext() {
		double r = 1.0 - m_value - 0.0000001;
		if(m_invBase < r) {
			m_value += m_invBase;
		}
		else {
			float h = m_invBase;
			float hh;
			do {
				hh = h;
				h *= m_invBase;
			} while(h >= r);
			m_value += hh + h - 1.0;
		}
		//std::cout << m_value << "\n";
		return m_value;
	}

private:
	float m_invBase;
	float m_value;
};

class Halton2DSampler : public Sampler {
public:
	Halton2DSampler(int base1, int base2) : m_halton1(base1), m_halton2(base2)
	{
	}

	/*void number(int i,int base) {
		double f = m_invBase = 1.0/base;
		value = 0.0;
		while(i>0) {
			value += f*(double)(i%base);
			i /= base;
			f *= inv_base;
		}
	}*/

	float getNext() {
		return m_halton1.getNext();
	}

	void getNext2D(float &x, float &y) {
		x = m_halton1.getNext();
		y = m_halton2.getNext();
	}

private:
	Halton1DSampler m_halton1, m_halton2;
};

/*struct ProgressivePixel
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
};*/

class PathRenderer;

/*class RenderEventHandler
{
public:
	virtual void onFrameRendered() 
	{}

	virtual void onLineRendered()
	{}

	virtual void onRenderStarted() 
	{}

	virtual void onRenderStopped()
	{}

	static RenderEventHandler defaultHandler;
};*/

//RenderEventHandler RenderEventHandler::defaultHandler;

class PathRenderer
{
public:
	PathRenderer() : 
		m_params(), m_scene(NULL), m_film(NULL),
		m_samples(0), m_frames(0), m_lines(0), 
		m_started(false), m_finished(false)
		//m_eventHandler(RenderEventHandler::defaultHandler)
	{
		m_subsampleSampler = new SimpleSampler;
		m_lightSelectionSampler = new Halton2DSampler(7, 2);
		m_bxdfSampler = new SimpleSampler;
		m_lightSampler = new SimpleSampler;
	}

	~PathRenderer() {
		delete m_subsampleSampler;
		delete m_lightSelectionSampler;
		delete m_bxdfSampler;
		delete m_lightSampler;
	}

	void render(Scene const& scene,
		Film& film,
		RenderParameters const& renderParameters);

	/*void setEventHandler(RenderEventHandler &handler) {
		m_eventHandler = handler;
	}*/
	
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
		//m_eventHandler.onRenderStopped();
	}

private:

	// private methods
	void renderProgressive();
	void renderScanline();
	Vec samplePixel(float x, float y, bool debug = false);
	Vec samplePixelProgressive(int px, int py, float &x, float &y, bool debug);
	Vec trace(Ray const& ray, unsigned int depth, bool seeLight = true, bool debug = false);
	Vec evaluateDirectLighting(Intersection const &isect);

	RenderParameters const *m_params;
	Scene const *m_scene;
	Film *m_film;

	Sampler *m_subsampleSampler;
	Sampler *m_lightSelectionSampler;
	Sampler *m_bxdfSampler;
	Sampler *m_lightSampler;

	unsigned int m_samples;
	unsigned int m_frames;
	unsigned int m_lines;
	bool m_started;
	bool m_finished;

	//RenderEventHandler &m_eventHandler;
};

#endif
#ifndef FILM_HPP
#define FILM_HPP

#include "vec.hpp"
#include "util.hpp"

class Film
{
public:
	struct PixelSample
	{
		Vec accum;
		float filterAccum;
	};

	Film(int width, int height) : 
		m_width(width), 
		m_height(height),
		m_filterWidth(2),
		m_filterHeight(2)
	{
		m_buffer = new PixelSample[width*height];
		for (int i = 0; i < width*height; ++i) {
			m_buffer[i].accum = nullVec;
			m_buffer[i].filterAccum = 0.f;
		}
	}

	~Film() 
	{
		delete [] m_buffer;
	}
	
	unsigned int getWidth() const {
		return m_width;
	}

	unsigned int getHeight() const {
		return m_height;
	}

	float evalFilter1D(float x)
	{
		float const B = 1.5f;
		float const C = 1.25f;
		x = fabsf(2.f * x);
		if (x > 1.f)
			return ((-B - 6*C) * x*x*x + (6*B + 30*C) * x*x +
			(-12*B - 48*C) * x + (8*B + 24*C)) * (1.f/6.f);
		else
			return ((12 - 9*B - 6*C) * x*x*x +
			(-18 + 12*B + 6*C) * x*x +
			(6 - 2*B)) * (1.f/6.f);
	}

	float evalFilter(float x, float y)
	{
		// TODO
		return evalFilter1D(x / m_filterWidth) * evalFilter1D(y / m_filterHeight);
	}

	PixelSample &pixelAt(int x, int y) const {
		return m_buffer[y*m_width + x];
	}

	void addSample(float x, float y, Vec const &value) 
	{
		int x1 = ceil2Int(x - m_filterWidth);
		int x2 = floor2Int(x + m_filterWidth);
		int y1 = ceil2Int(y - m_filterHeight);
		int y2 = ceil2Int(y + m_filterWidth);
		x1 = std::max(0, x1);
		x2 = std::min(x2, m_width);
		y1 = std::max(0, y1);
		y2 = std::min(y2, m_height);
		// update all pixels affected by the sample
		for (int ix = x1; ix < x2; ++ix) {
			for (int iy = y1; iy < y2; ++iy) {
				float filterVal = evalFilter(
					fabsf(static_cast<float>(ix) - x),
					fabsf(static_cast<float>(iy) - y));
				pixelAt(ix, iy).accum += filterVal * value;
				pixelAt(ix, iy).filterAccum += filterVal;
			}
		}
	}

	void convertToRGB(uint32_t *outPixels) const {
		for (unsigned int x = 0; x < m_width; ++x) {
			for (unsigned int y = 0; y < m_height; ++y) {
				PixelSample &sample = pixelAt(x, y);
				if (sample.filterAccum != 0) {
					outPixels[x + y*m_width] = mapRGB(sample.accum / sample.filterAccum);
				} else {
					outPixels[x + y*m_width] = mapRGB(Vec());
				}
			}
		}
	}
	
private:
	PixelSample *m_buffer;
	int m_width;
	int m_height;
	float m_filterWidth;
	float m_filterHeight;
};

#endif
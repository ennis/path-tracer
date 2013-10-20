#ifndef ENVMAP_HPP
#define ENVMAP_HPP

#include <iostream>
#include <cmath>
#include "vec.hpp"
#include "constants.hpp"
#include "rgbe/rgbe.hpp"

class EnvironmentMap
{
public:
	EnvironmentMap() : m_data(NULL), m_width(0), m_height(0), m_scale(0.f)
	{}

	EnvironmentMap(unsigned int width, unsigned int height) : 
		m_width(width),
		m_height(height)
	{
		m_data = new float[width * height * 3];
	}

	~EnvironmentMap() {
		delete [] m_data;
	}

	void loadFromFile(char const *fileName, float scaleFactor)
	{
		FILE * f = fopen(fileName, "rb");
		// TODO error checking
		m_scale = scaleFactor;
		RGBE_ReadHeader(f, &m_width, &m_height, NULL);
		m_data = new float[m_width * m_height * 3];
		RGBE_ReadPixels_RLE(f, m_data, m_width, m_height);
		fclose(f);
		std::clog << "Read envmap " << fileName << ", width=" << m_width << ", height=" << m_height << "\n";
	}

	Vec eval(Vec const& dir) const 
	{
		float u = 1.f + atan2f(dir.x(), -dir.z()) / M_PI;
		float v = acosf(dir.y()) / M_PI;
		// ( 1 + atan2(Dx,-Dz) / pi, arccos(Dy) / pi)

		int x = static_cast<int>(static_cast<float>(m_width) * u / 2.f);
		int y = static_cast<int>(static_cast<float>(m_height) * v);
		if (x >= m_width) 
			x = m_width - 1;
		if (y >= m_height) 
			y = m_height - 1;

		unsigned int offset = (y * m_width + x) * 3;
		float r = m_data[offset + 0];
		float g = m_data[offset + 1];
		float b = m_data[offset + 2];
		// TODO cosine falloff
		
		//std::clog << Vec(r, g, b) << "\n";
		return Vec(r, g, b) * m_scale;
	}

private:
	float *m_data;	
	int m_width, m_height;
	float m_scale;
};

#endif
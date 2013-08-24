#pragma once
#include "util.hpp"
#include "vec.hpp"
#include <cmath>

class Texture
{
public:
	virtual Vec sample(float u, float v) const {
		return Vec(1.f,1.f,1.f);
	}

private:
};


//===================================
// ColorTexture
class ColorTexture : public Texture
{
public:
	ColorTexture(Vec const& color) : m_color(color)
	{}

	virtual Vec sample(float u, float v) const {
		return m_color;
	}

private:
	Vec m_color;
};


//===================================
// CheckerboardTexture
class CheckerboardTexture : public Texture
{
public:
	CheckerboardTexture(
		Vec const& color1, 
		Vec const& color2, 
		float width, 
		float height) : 
		m_color1(color1),
		m_color2(color2),
		m_width(width),
		m_height(height)
	{}

	virtual Vec sample(float u, float v) const {
		return ((fmodf(u / m_width, 1.f) < 0.5f) ^
			(fmodf(v / m_height, 1.f) < 0.5f)) ? 
			m_color1 : m_color2;
	}

private:
	Vec m_color1;
	Vec m_color2;
	float m_width, m_height;
};

//===================================
// GridTexture
class GridTexture : public Texture
{
public:
	GridTexture(
		Vec const& color1, 
		Vec const& color2, 
		float width, 
		float height) : 
		m_color1(color1),
		m_color2(color2),
		m_width(width),
		m_height(height)
	{}

	virtual Vec sample(float u, float v) const {
		float su = fmodf(m_width * u, 1.f);
		float sv = fmodf(m_height * v, 1.f);
		
		if (su < 0.01f || sv < 0.01f) {
			return m_color1;
		} else {
			return m_color2;
		}
	}

private:
	Vec m_color1;
	Vec m_color2;
	float m_width, m_height;
};

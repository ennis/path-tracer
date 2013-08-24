#ifndef UTIL_HPP
#define UTIL_HPP
#include <cmath>
#include "vec.hpp"

static inline 
float frand(float a, float b)
{
  return a + ((float)rand() / (float)RAND_MAX) * (b-a);
}

static inline uint32_t mapRGB(Vec const& c)
{
	Vec cl = clamp(c, 0.f, 1.f);
	uint8_t r = static_cast<uint8_t>(cl.x()*255.f);
	uint8_t g = static_cast<uint8_t>(cl.y()*255.f);
	uint8_t b = static_cast<uint8_t>(cl.z()*255.f);
	return r | g << 8 | b << 16 | 0xFF << 24;
}

template <typename T>
T clamp(T v, T min, T max)
{
	return std::min(std::max(v, min), max);
}

static inline
int ceil2Int(float v)
{
	return static_cast<int>(ceilf(v));
}

static inline 
int floor2Int(float v)
{
	return static_cast<int>(floorf(v));
}

#endif


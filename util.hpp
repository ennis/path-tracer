#ifndef UTIL_HPP
#define UTIL_HPP
#include <cmath>

static inline 
float frand(float a, float b)
{
  return a + ((float)rand() / (float)RAND_MAX) * (b-a);
}

#endif


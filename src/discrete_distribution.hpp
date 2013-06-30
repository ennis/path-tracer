#ifndef DISCRETE_DISTRIBUTION_HPP
#define DISCRETE_DISTRIBUTION_HPP

#include <cassert>
#include <iostream>
#include "util.hpp"

static int simulateCdf(int numEvents, float const *cdf, float u)
{
	float x = frand(0.f, 1.f) * u;
	float last = 0;
	for (int i = 0; i < numEvents; ++i) {
		if (last < x && cdf[i] >= x) {
			return i;
		} else {
			last = cdf[i];
		}
	}
	return numEvents - 1;
}

#endif
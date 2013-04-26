#pragma once
#include "vec.hpp"

struct Ray {
  Ray() {}
  Ray(Point const& origin, Vec const& direction) : O(origin), D(direction.normalized()) {}
  Point O;
  Vec D;
};

Vec reflectedRay(Vec const& N, Vec const& I);
Vec refractedRay(Vec const& N, Vec const& I, float n);
Vec scatteredReflectedRay(Vec const& N, Vec const& I);
Vec specularRay(Vec const& N, Vec const& I, float specular, bool& bounce);
Vec uniformRandomRay(Vec const& N);
Vec halfway(Vec const& in, Vec const& out);
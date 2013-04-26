#include "ray.hpp"
#include "vec.hpp"
#include "util.hpp"
#include <cmath>

Vec reflectedRay(Vec const& N, Vec const& I)
{
  Vec O = I - 2*N*dot(I, N);
  return O.normalized();
}

Vec refractedRay(Vec const& N, Vec const& I, float n)
{
  Vec In = I.normalized();
  Vec Nn = N.normalized();
  float cosi = -dot(In,Nn);
  float rad = 1-(1-cosi*cosi)/(n*n);
  if (rad < 0) {
    // total internal reflection
    //std::cout << "T.I.R.\n";
    return reflectedRay(N, I);
  }
  float Nc = sqrt(rad) - cosi/n;
  float Ic = 1/n;
  //std::cout << Ic << ' , ' << Nc << '\n';
  return Ic * In - Nc * Nn;
}

Vec scatteredReflectedRay(Vec const& N, Vec const& I)
{
  Vec O = I - 2*N*dot(I, N*frand(0.9,1.1));
  //O.x += frand(0.0, 0.1);
  // slightly rotate vector?
  return O.normalized();
}

Vec specularRay(Vec const& N, Vec const& I, float specCoef, bool& bounce)
{
  // generate rays around reflected ray
  Vec O = I - 2*N*dot(I, N);
  Vec T, S;
  genOrtho(O, T, S);
  float z = sqrt(1-pow(frand(0,1), specCoef)); // non-uniform sampling; biased for reflection
  float theta = acos(z);
  float phi = frand(0, 2.0f*M_PI);
  //Vec V = Vec(sin(theta)*cos(phi), sin(theta)*sin(phi), z);
  Vec Vt = sin(theta)*cos(phi) * T + sin(theta)*sin(phi) * S + z * O;

  // out of hemisphere?
  if (dot(Vt,N) < 0.0) {
    bounce = false;
    return Vec(0,0,0);
  }
  bounce = true;
  return Vt.normalized();
}

Vec uniformRandomRay(Vec const& N)
{
  Vec T, S;
  genOrtho(N, T, S);
  float z = sqrt(frand(0,1));
  float theta = acos(z);
  float phi = frand(0, 2.0f*M_PI);
  //Vec V = Vec(sin(theta)*cos(phi), sin(theta)*sin(phi), z);
  Vec Vt = sin(theta)*cos(phi) * T + sin(theta)*sin(phi) * S + z * N;
  return Vt.normalized();
}

Vec halfway(Vec const& in, Vec const& out)
{
  return (out+in).normalized();
}
#include "ray.hpp"
#include "vec.hpp"
#include "util.hpp"
#include <cmath>

Vec perfectSpecularReflection(Vec const& I)
{
	return I * Vec(-1.f, -1.f, 1.f);
}


Vec perfectSpecularRefraction(Vec const& I, float n)
{
	float cosi = I.z();
	float rad = 1-(1-cosi*cosi)/(n*n);
	if (rad < 0) {
		// total internal reflection
		//std::cout << "T.I.R.\n";
		return perfectSpecularReflection(I);
	}
	float Nc = sqrt(rad) - cosi/n;
	float Ic = 1/n;
	//std::cout << Ic << ' , ' << Nc << '\n';
	return - Ic * I - Vec(0.f, 0.f, Nc);
}

Vec reflectedRay(Vec const& N, Vec const& I)
{
  Vec O = 2*N*dot(I, N) - I;
  return O.normalized();
}

Vec refractedRay(Vec const& N, Vec const& I, float n)
{
	float cosi = dot(I,N);
	float rad = 1-(1-cosi*cosi)/(n*n);
	if (rad < 0) {
		// total internal reflection
		//std::cout << "T.I.R.\n";
		return reflectedRay(N, I);
	}
	float Nc = sqrt(rad) - cosi/n;
	float Ic = 1/n;
	//std::cout << Ic << ' , ' << Nc << '\n';
	return - Ic * I - Nc * N;
}

Vec scatteredReflectedRay(Vec const& N, Vec const& I)
{
	Vec O = I - 2*N*dot(I, N*frand(0.5,1.5));
	//O.x += frand(0.0, 0.1);
	// slightly rotate vector?
	return O.normalized();
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

Vec sampleCosineWeightedRay(Vec const& N, float sampleX, float sampleY)
{
  Vec T, S;
  genOrtho(N, T, S);
  float z = sampleX;
  float r = sqrt(z);
  float theta = sampleY * 2.0f * M_PI;
  //Vec V = Vec(sin(theta)*cos(phi), sin(theta)*sin(phi), z);
  Vec Vt = r * cos(theta) * T + r * sin(theta) * S + sqrt(1-z) * N;
  return Vt;
}

Vec sampleUniformRandomRay(Vec const& N, float sampleX, float sampleY)
{
	Vec T, S;
	genOrtho(N, T, S);
	float sr2 = sqrt(1-sampleY*sampleY);
	float phi = sampleX * 2.0f * M_PI;
	Vec Vt = sr2*cos(phi) * T + sr2*sin(phi) * S + sampleY * N;
	return Vt;
}

Vec cosinePowerSampleHemisphere(float exp, float u1, float u2)
{
	float sample = u1;
	float phi = 2.f * M_PI * u2;
	float sr2 = sqrtf(1 - pow(sample, 2 / (exp + 1)));
	return Vec(sr2 * cos(phi), sr2 * sin(phi), pow(sample, 1 / (exp + 1)));
}

Vec halfway(Vec const& Wi, Vec const& Wo)
{
  return (Wo+Wi).normalized();
}

Vec cosineSampleHemisphere(float u1, float u2)
{
	float z = u1;
	float r = sqrtf(z);
	float theta = u2 * 2.0f * M_PI;
	return Vec(r * cos(theta), r * sin(theta), sqrt(1-z));
}

Vec uniformSampleHemisphere(float u1, float u2)
{
	float z = sqrtf(u1);
	float theta = acos(z);
	float phi = 2.f * M_PI * u2;
	return Vec(sin(theta)*cos(phi), sin(theta)*sin(phi), z);
}



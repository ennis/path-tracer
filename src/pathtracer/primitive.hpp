#ifndef PRIMITIVE_HPP
#define PRIMITIVE_HPP

#include "vec.hpp"
#include "bbox.hpp"
#include "texture.hpp"
#include "transform.hpp"
#include "ray.hpp"

#include <iostream>

class Primitive;
class Material;

//==================================
// Intersection
struct Intersection
{
	// Primitive at hit point
	Primitive const *primitive;
	// incoming ray direction
	Vec WoW;
	// distance travelled along the ray
	float t;
	// hit point
	Point P;
	// Local base
	Vec N, T, S;
	// Texture coordinates
	float u, v;

	inline Vec toLocal(Vec const& VW) const
	{
		return Vec(dot(T,VW), 
			dot(S,VW),
			dot(N,VW));
	}

	inline Vec toWorld(Vec const& VL) const
	{
		return VL.x() * T + 
				VL.y() * S +
				VL.z() * N;
	}
	
	// Lambertian, phong: Sampled texture
	Vec texSample;
};

enum LightSamplingStrategy
{
	SOLID_ANGLE,
	AREA_LIGHT,
	POINT_LIGHT
};

struct OcclusionTest
{
	float maxt;
	Ray shadow;
};

//==================================
// Emitter
class Emitter
{
public:
	// sample the light
	virtual Vec sampleLight(Point const &P, float u1, float u2, Vec &WiW, float &pdf, float &maxt) const {
		return Vec();
	}

protected:
};

//==================================
// PointLight
class PointLight : public Emitter
{
public:
	PointLight(Point const &at, Vec const &emittance) : m_point(at), m_emittance(emittance)
	{}

	virtual Vec sampleLight(Point const &P, float u1, float u2, Vec &WiW, float &pdf, float &maxt) const {
		Vec D = m_point - P;
		float d2 = dot(D, D);
		maxt = sqrtf(d2);
		WiW = D / maxt;
		pdf = 1.f;
		return m_emittance / d2;
		//std::clog << E << '\n';
	}

protected:
	Point m_point;
	Vec m_emittance;
};

//==================================
// Primitive
class Primitive : public Emitter
{
public:
	Primitive(
		Transform const *transform, 
		Material const *material,
		Vec const &emittance) :
	m_transform(transform),
	m_material(material),
	m_emittance(emittance)
	{}

	virtual ~Primitive()
	{}

	/*
	 * Compute intersection point with primitive
	 * If isect is NULL, only a predicate is returned
	 */
	virtual bool intersect(Ray const& ray, Intersection& isect) const = 0;
	
	Material const *getMaterial() const {
		return m_material;
	}

	Vec const &getEmittance() const {
		return m_emittance;
	}

protected:
	AABB m_aabb;
	Transform const *m_transform;
	Material const *m_material;
	Vec m_emittance;
};

static inline void sphereUV(Vec const& N, float &u, float &v)
{
	// UV parameters
	u = 0.5f + atan2f(N.z(), N.x()) / (2 * M_PI);
	v = 0.5f + 2.f * asinf(N.y()) / (2 * M_PI);
}

class Sphere : public Primitive
{
public:
	Sphere(Point const& center,
		float radius, 
		Transform const *transform, 
		Material const *material,
		Vec const &emittance) :
	Primitive(transform, material, emittance),
	m_center(center),
	m_radius(radius)
	{
		// TODO AABBs
		/*m_aabb = AABB(Point(-radius, -radius, -radius), 
						Point(radius, radius, radius));
		if (transform != NULL) {
			m_aabb = transformAABB(*transform, m_aabb);
		}*/
	}

	virtual ~Sphere() 
	{}

	virtual bool intersect(Ray const& R, Intersection& isect) const
	{
		// Ray to object space
		Vec RO = R.O - m_center;

		// Compute A, B and C coefficients
		float a = dot(R.D, R.D);
		float b = 2 * dot(R.D, RO);
		float c = dot(RO, RO) - (m_radius * m_radius);
		//std::clog << a << ' ' << b << ' ' << c << std::endl;
		// Find discriminant
		float disc = b*b - 4*a*c;
		// if discriminant is negative there are no real roots, so return 
		// false as R misses sphere
		if (disc < 0) 
			return false;
		//std::clog << disc << std::endl;
		// compute q as described above
		float discSqrt = sqrtf(disc);

		float q;
		if (b < 0.f) {
			q = -0.5f * (b - discSqrt);
		} else {
			q = -0.5f * (b + discSqrt);
		}

		float t0 = q / a;
		float t1 = c / q;

		if (t0 > t1) {
			std::swap(t0, t1);
		}

		if (t1 < EPSILON)
			return false;

		if (t0 < EPSILON) {
			isect.t = t1;
		}
		else {
			isect.t = t0;
		}
		
		isect.P = R.along(isect.t);
		isect.N = (isect.P - m_center).normalized();
		//isect.P += EPSILON * isect.N;

		// UV parameters
		sphereUV(isect.N, isect.u, isect.v);
		
		genOrtho(isect.N, isect.S, isect.T);
		isect.primitive = this;
		return true;
	}

	virtual Vec sampleLight(Point const &P, float u1, float u2, Vec &WiW, float &pdf, float &maxt) const {
		Vec D = m_center - P;
		Vec N = D.normalized();
		float d2 = dot(D, D);
		// apparent angle
		float cos_max = sqrtf(1.f - (m_radius*m_radius / d2));
		// solid angle
		float omega = 2*M_PI*(1-cos_max);
		// generate a ray 
		Vec T,S;
		genOrtho(N, T, S);

		float phi = 2*M_PI*u1;
		float v = 1.f - u2 * (1 - cos_max);
		float w = sqrt(1 - v*v);
		WiW = cos(phi) * w * T + sin(phi) * w * S + v * N;
		Ray R = Ray(P, WiW);
		Intersection test;
		if (!intersect(R, test)) {
			//std::clog << "GRAZE.\n";
			pdf = 1.f / omega;
			return Vec();
		}

		maxt = (P - test.P).norm();
		pdf = 1.f / omega;

		return m_emittance;

		//pdf =  omega / (4.0f * M_PI);	// INV_TWOPI
		//std::clog << omega << '\t' << pdf << '\n';
	}

protected:
	Point m_center;
	float m_radius;
};


class Plane : public Primitive
{
public:
	Plane(Point const& center,
		Vec const& normal, 
		Transform const *transform, 
		Material *material,
		Vec const &emittance) :
	Primitive(transform, material, emittance),
	m_center(center),
	m_normal(normal)
	{}

	virtual ~Plane() 
	{}

	virtual bool intersect(Ray const& R, Intersection& isect) const
	{
		if (fabs(dot(R.D, m_normal)) < EPSILON) {
			return false;
		} else {
			isect.t = dot(m_center - R.O, m_normal) / dot(R.D, m_normal);
			if (isect.t < EPSILON) {
				return false;
			}
		}
		isect.N = m_normal;
		
		// TODO factorize
		genOrtho(isect.N, isect.S, isect.T);
		isect.P = R.O + isect.t * R.D;
		isect.primitive = this;

		Vec PL = isect.toLocal(isect.P);
		isect.u = (PL.x() < 0.f) ? (1.f - fmodf(abs(PL.x()), 1.f)) : fmodf(PL.x(), 1.f);
		isect.v = (PL.y() < 0.f) ? (1.f - fmodf(abs(PL.y()), 1.f)) : fmodf(PL.y(), 1.f);

		return true;
	}

	virtual void sample(Point const& O, Intersection &isect, float &pdf) const {
		// TODO
	}

protected:
	Point m_center;
	Vec m_normal;
};


#endif
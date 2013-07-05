#ifndef PRIMITIVE_HPP
#define PRIMITIVE_HPP

#include "vec.hpp"
#include "bbox.hpp"
#include "bsdf.hpp"
#include "texture.hpp"
#include "transform.hpp"
#include "ray.hpp"

class Primitive;

//==================================
// Intersection
struct Intersection
{
	// Primitive at hit point
	Primitive const *primitive;
	// distance travelled along the ray
	float t;
	// hit point
	Point P;
	// Local base
	Vec N, T, S;
	// Texture coordinates
	float u, v;

	inline Vec worldToLocal(Vec const& VW) const
	{
		return Vec(dot(T,VW), 
			dot(S,VW),
			dot(N,VW));
	}

	inline Vec localToWorld(Vec const& VL) const
	{
		return VL.x() * T + 
				VL.y() * S +
				VL.z() * N;
	}
};

//==================================
// Emitter
class Emitter
{
public:
	// sample the light
	virtual void sampleL(Point const &P, Vec &Wi, Vec &E, float &pdf) const = 0;
protected:
};

//==================================
// PointLight
class PointLight : public Emitter
{
public:
	virtual void sampleL(Point const &P, Vec &Wi, Vec &E, float &pdf) const {
		Wi = m_point - P;
		E = m_emittance / dot(Wi, Wi);
		Wi = Wi.normalized();
		pdf = 1.f;
	}

protected:
	Point const &m_point;
	Vec const &m_emittance;
};

//==================================
// Primitive
class Primitive
{
public:
	Primitive(Transform const *transform, 
		Texture const *texture,
		BxDF const *bxdf,
		Vec const &emittance) :
	m_transform(transform),
	m_texture(texture),
	m_bxdf(bxdf),
	m_emittance(emittance)
	{}

	virtual ~Primitive()
	{}

	/*
	 * Compute intersection point with primitive
	 * If isect is NULL, only a predicate is returned
	 */
	virtual bool intersect(Ray const& ray, Intersection& isect) const = 0;
	
	/*
	 * Sample a point on the primitive
	 */
	virtual void sample(Point const& O, Intersection &isect, float &pdf) const = 0;

	/*
	 * Emitter : sample 
	 */
	void sampleL(Point const &P, Vec &Wi, Vec &E, float &pdf) const {
		Intersection isect;
		sample(P, isect, pdf);
		// TODO texture
		E = m_emittance * M_PI / pdf;
		Wi = P - isect.P;
	}

	Texture const *getTexture() const {
		return m_texture;
	}
	
	BxDF const *getBxDF() const {
		return m_bxdf;
	}

	Vec const &getEmittance() const {
		return m_emittance;
	}

protected:
	AABB m_aabb;
	// TODO transform, texture, material in base Primitive class
	Transform const *m_transform;
	Texture const *m_texture;
	BxDF const *m_bxdf;
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
		Texture const *texture,
		BxDF const *bxdf,
		Vec const &emittance) :
	Primitive(transform, texture, bxdf, emittance),
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
		float a = 1.f;
		float b = 2 * dot(R.D, RO);
		float c = dot(RO, RO) - (m_radius * m_radius);
		//std::clog << a << ' ' << b << ' ' << c << std::endl;
		// Find discriminant
		float disc = b*b - 4*a*c;
		// if discriminant is negative there are no real roots, so return 
		// false as R misses sphere
		if (disc < EPSILON) 
			return false;
		//std::clog << disc << std::endl;
		// compute q as described above
		float discSqrt = sqrt(disc);
		float t1 = (-b + discSqrt)/(2.0f*a);
		if (t1 < EPSILON)
			return false;
		float t0 = (-b - discSqrt)/(2.0f*a);
		if (t0 < EPSILON) {
			isect.t = t1;
		}
		else {
			isect.t = t0;
		}
		
		isect.P = R.O + isect.t * R.D;
		isect.N = (isect.P - m_center).normalized();

		// UV parameters
		sphereUV(isect.N, isect.u, isect.v);
		
		genOrtho(isect.N, isect.S, isect.T);
		isect.primitive = this;
		return true;
	}

	virtual void sample(Point const& O, Intersection &isect, float &pdf) const {
		// sample a point on the sphere
		Vec N = (O - m_center).normalized();
		Vec D = uniformSampleHemisphere();
		Vec T, S;
		genOrtho(N, T, S);
		isect.P = m_center + m_radius * (D.x() * T + D.y() * S + D.z() * N);
		isect.N = N;
		isect.T = T;
		isect.S = S;
		pdf = 1.f / (2.f * M_PI);	// INV_TWOPI
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
		Texture const *texture,
		BxDF const *bxdf,
		Vec const &emittance) :
	Primitive(transform, texture, bxdf, emittance),
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

		// TODO plane UV mapping
		
		// TODO factorize
		genOrtho(isect.N, isect.S, isect.T);
		isect.P = R.O + isect.t * R.D;
		isect.primitive = this;
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
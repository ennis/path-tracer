#ifndef PRIMITIVE_HPP
#define PRIMITIVE_HPP

#include "vec.hpp"
#include "bsdf.hpp"
#include "texture.hpp"
#include "transform.hpp"
#include "geometry.hpp"

class Primitive;

//==================================
// Intersection
struct Intersection
{
	// Material at hit point
	Material const *material;
	// Texture object
	Texture const *texture;
	// Local geometry
	LocalGeometry localGeometry;
};

//==================================
// Primitive
class Primitive
{
public:
	Primitive()
	{}

	virtual ~Primitive()
	{}

	/*
	 * Compute intersection point with primitive
	 * If isect is NULL, only a predicate is returned
	 */
	virtual bool intersect(Ray const& ray, Intersection * isect) const = 0;

	/*
	 * Get AABB of primitive
	 * Returns: true if primitive has an AABB, false otherwise
	 * The contents of 'aabb' are undefined in this case
	 */
	virtual bool getAABB(AABB& aabb) {
		return false;
	}
};

class Sphere : public Primitive
{
public:
	Sphere(Vec const& center,
			float radius, 
			Transform const *transform, 
			Texture const *texture,
			BxDF const *bxdf) :
	m_center(center)
	m_radius(radius),
	m_transform(transform),
	m_texture(texture),
	m_bxdf(bxdf)
	{
		m_aabb = AABB(Point(-radius, -radius, -radius), 
						Point(radius, radius, radius)));
		if (transform != NULL) {
			m_aabb = transformAABB(m_aabb);
		}
	}

	virtual ~Sphere() {}

	virtual bool getAABB(AABB& aabb) 
	{
		aabb = m_aabb;
		return true;
	}

	virtual bool intersect(Ray const& R, Intersection * isect) 
	{
		Ray RO = R;
		// Ray to object space
		RO.O -= m_center;
		raySphereIntersection(RO, localGeom);
		// world space
		localGeom.P += m_center;
		return true;
	}

protected:
	Point m_center;
	float m_radius;
	AABB m_aabb;
	Transform const *m_transform;
	Texture const *m_texture;
	BxDF const *m_bxdf;
};

#endif
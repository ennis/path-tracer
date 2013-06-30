#pragma once
#include "vec.hpp"
#include "bsdf.hpp"
#include "texture.hpp"
#include "matrix4x4.hpp"
#include "material.hpp"
#include "geometry.hpp"


class Primitive
{
public:
	virtual ~Primitive()
	{}

	virtual Material const* getMaterial() const {
		return NULL;
	}

	virtual bool intersect(Ray const& ray, Intersection * isect, Material const** material) const = 0;

protected:
};


class GeometricPrimitive : public Primitive
{
public:
	GeometricPrimitive(Geometry const* geometry,
					   Material const* material,
					   Texture const* texture,
					   Transform const& worldToObject) : 
	m_geometry(geometry),
	m_material(material),
	m_texture(texture),
	m_worldToObject(worldToObject)
	{}

	virtual ~GeometricPrimitive() 
	{}

	virtual Material const* getMaterial() const {
		return m_material;
	}

	virtual Geometry const* getGeometry() const {
		return m_geometry;
	}

	virtual Texture const* getTexture() const {
		return m_texture;
	}

	virtual bool intersect(Ray const& ray, Intersection * isect, Material const** material) const {
		*material = m_material;
		return m_geometry->intersect(ray, isect);
	}

	virtual Transform const& getTransform() const {
		return m_worldToObject;
	}

private:
	Geometry const *m_geometry;
	Material const *m_material;
	Texture const *m_texture;
	Transform m_worldToObject;
};

#pragma once
#include "vec.hpp"
#include "geometry.hpp"
#include "bsdf.hpp"

/*
 Object: material + geometry
 */

class Object
{
public:
	Object(Point const& position,
			Vec const& color, 
			Vec const& emittance,
			Geometry const* geometry,
			BSDF const* bsdf) : 
	m_position(position),
	m_color(color),
	m_emittance(emittance),
	m_geometry(geometry),
	m_bsdf(bsdf)
	{}

	bool isLightSource() const {
		return m_emittance.x() != 0.f && 
				m_emittance.y() != 0.f && 
				m_emittance.z() != 0.f;
	}

	Point const& getPosition() const {
		return m_position;
	}

	Vec const& getEmittance() const {
		return m_emittance;
	}

	Vec const& getColor() const {
		return m_color;
	}

	Geometry const *getGeometry() const {
		return m_geometry;
	}

	BSDF const* getBSDF() const {
		return m_bsdf;
	}

private:
	Point m_position;
	Vec m_color;
	Vec m_emittance;
	Geometry const *m_geometry;
	BSDF const *m_bsdf;
	//Transform *transform;
	//Texture *texture;
};

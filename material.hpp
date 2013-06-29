#ifndef MATERIAL_HPP
#define MATERIAL_HPP 
#include "vec.hpp"
#include "ray.hpp"
#include "bsdf.hpp"
#include "texture.hpp"

class Material
{
public:
	Material(Texture const* texture, Vec const& emittance, bool lightSource, BSDF const* bsdf) : 
		m_texture(texture),
		m_emittance(emittance),
		m_isLightSource(lightSource),
		m_bsdf(bsdf)
	{}

	~Material()
	{}


	Vec const& getEmittance() const {
		return m_emittance;
	}

	BSDF const* getBSDF() const {
		return m_bsdf;
	}

	bool isLightSource() const {
		return m_isLightSource;
	}

private:
	Vec m_emittance;
	bool m_isLightSource;
	// TODO multiple BSDFs
	BSDF const *m_bsdf;
};


#endif
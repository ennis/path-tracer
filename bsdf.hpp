#pragma once
#include "ray.hpp"
#include "discrete_distribution.hpp"
#include <iostream>

static const unsigned int MAX_BSDF_SAMPLES = 4;

enum BxDFType
{
	BxDF_REFLECTION,
	BxDF_TRANSMISSION,
	BxDF_DIFFUSE,
	BxDF_SPECULAR,
	BxDF_GLOSSY
};

class BxDF
{
public:
	BxDF(BxDFType& bxdfType) : type(BxDFType)
	{}
	
	/*
	 * All vectors in local geometry space
	 * Normal vector is (0,0,1)
	 */
	virtual void sample(Vec const& color, Vec const& Wi, Vec& Wo, Vec& bsdfResult, float& pdfResult) const = 0;
	virtual void eval(Vec const& color, Vec const& Wi, Vec const& Wo, Vec& bsdfResult, float& pdfResult) const = 0;
	//virtual float weight(Vec const& Wi) const = 0;

	const BxDFType type;

	static inline float cosTheta(Vec const& W) {
		return W.z();
	}

private:
};

class BSDF
{
public:
	/*
	 * getNumComponents
	 * Returns the number of components (number of BxDFs to choose from) of the BSDF
	 */
	virtual int getNumComponents() const = 0;
	/*
	 * sample
	 * bxdfId: BxDF to sample (0..numComponents-1) or -1 to choose at random
	 * color: sampled texture color
	 * N: surface normal in world coordinates
	 * Wi: incoming direction in world space
	 * Wo: sampled direction in world space
	 * bsdfResult: BSDF value 
	 * bxdfType: type of the sampled BxDF
	 */
	virtual void sample(int bxdfId, 
						Vec const& color,
						Vec const& N, 
						Vec const& Wi, 
						Vec& Wo, 
						Vec& bsdfResult, 
						float& pdfResult, 
						BxDFType& bxdfType) const = 0;
	/*
	 * eval: evaluates the BSDF with given incoming and outgoing directions
	 * bxdfId: BxDF to sample (0..numComponents)
	 * color: sampled texture color
	 * N: surface normal in world coordinates
	 * Wi: incoming direction in world space
	 * Wo: outgoing direction in world space
	 * bsdfResult: BSDF value 
	 */
	virtual void eval(int bxdfId, 
						Vec const& color, 
						Vec const& N, 
						Vec const& Wi, 
						Vec const& Wo, 
						Vec& bsdfResult) const = 0;
};

//===================================
// Mirrors
class MirrorBRDF : public BxDF
{
public:
	MirrorBRDF() : BxDF(BxDF_REFLECTION | BxDF_SPECULAR) {}
	~MirrorBRDF() {}

	virtual void sample(Vec const& color,
						Vec const& Wi, 
						Vec& Wo, 
						Vec& bsdfResult,
						float& pdfResult) const 
	{
		bsdfResult = Vec(1.f, 1.f, 1.f);
		pdfResult = 1.f;	// should be a dirac?
		Wo = perfectSpecularReflection(Wi);
	}

	virtual void eval(Vec const& color,
					  Vec const& Wi, 
					  Vec const& Wo, 
					  Vec& bsdfResult,
					  float& pdfResult) const 
	{
		bsdfResult = Vec(0.f, 0.f, 0.f);
	}

private:
};

//===================================
// Lambertian diffuse reflection
class LambertianBRDF : public BxDF
{
public:
	LambertianBRDF() : BxDF(BxDF_REFLECTION | BxDF_DIFFUSE) {} 
	~LambertianBRDF() {}

	virtual void sample(Vec const& color,
						Vec const& Wi, 
						Vec& Wo, 
						Vec& bsdfResult,
						float& pdfResult) const 
	{
		bsdfResult = color * cosTheta(Wi);
		pdfResult = 1.f;
		Wo = cosineSampleHemisphere();
	}

	virtual void eval(Vec const& color,
					  Vec const& Wi, 
					  Vec const& Wo, 
					  Vec& bsdfResult,
					  float& pdfResult) const 
	{
		bsdfResult = color * cosTheta(Wi) * cosTheta(Wo);
		pdfResult = 1.f;
	}

private:
};

//===================================
// Phong specular highlights
class PhongBRDF : public BxDF
{
public:
	PhongBRDF(float phongExp, Vec const& highlightsColor) : 
		BxDF(BxDF_REFLECTION | BxDF_GLOSSY), 
		m_phongExp(phongExp), 
		m_highlightsColor(highlightsColor) 
	{} 
	~PhongBRDF() {}

	virtual void sample(Vec const& color,
						Vec const& Wi, 
						Vec& Wo, 
						Vec& bsdfResult,
						float& pdfResult) const 
	{
		pdfResult = 1.f;
		Wo = cosineSampleHemisphere();
		eval(color, Wi, Wo, bsdfResult, pdfResult);
	}

	virtual void eval(Vec const& color,
					  Vec const& Wi, 
					  Vec const& Wo, 
					  Vec& bsdfResult,
					  float& pdfResult) const 
	{
		Vec H = halfway(Wi, Wo);
		bsdfResult = m_highlightsColor * powf(H.z(), m_phongExp);
		pdfResult = 1.f;
	}

private:
	Vec m_highlightsColor;
	float m_phongExp;
};



//===================================
// MixBSDF
class MixBSDF : public BSDF
{
public:
	static const int MAX_BxDFs = 8;

	MixBSDF() : BSDF()
	{}

	virtual ~MixBSDF() 
	{}

	virtual int getNumComponents() const {
		return m_numBxdfs;
	}

	virtual bool add(BxDF const *bxdf, float weight) 
	{
		if (m_numBxdfs >= MAX_BxDFs) {
			std::cerr << "MixBSDF::add: too many BxDFs\n (maximum is " << MAX_BxDFs << ")\n";
			return false;
		}
		m_bxdfs[m_numBxdfs] = bxdf;
		m_cdf[m_numBxdfs] = m_cdf[m_numBxdfs-1] + weight;
		m_weights[m_numBxdfs] = weight;
		m_numBxdfs++;
		return true;
	}

	virtual void sample(int bxdfId, 
						Vec const& color,
						Vec const& N, 
						Vec const& Wi, 
						Vec& Wo, 
						Vec& bsdfResult, 
						float& pdfResult, 
						BxDFType& bxdfType) const 
	{

		if (bxdfId != -1) {
			std::cerr << "MixBSDF::sample: unimplemented\n";
			return;
		}
		bxdfId = simulateCdf(m_numBxdfs, m_cdf, m_cdf[m_numBxdfs-1]);
		m_bxdfs[bxdfId]->sample(color, Wi, Wo, bsdfResult, pdfResult);
		bxdfType = m_bxdfs[bxdfId]->type;
	}

private:
	float m_weights[MAX_BxDFs];
	float m_cdf[MAX_BxDFs];
	BxDF const *m_bxdfs[MAX_BxDFs];
	int m_numBxdfs;
};

#pragma once
#include "ray.hpp"
#include "util.hpp"
#include "vec.hpp"
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

// TODO Unified material class
class BxDF
{
public:
	
	/*
	 * All vectors in local geometry space
	 * Normal vector is (0,0,1)
	 */

	/*
	 * sample
	 * bxdfId: BxDF to sample (0..numComponents-1) or -1 to choose at random
	 * color: sampled texture color
	 * N: surface normal in Wirld coordinates
	 * Wo: incoming direction in Wirld space
	 * Wi: sampled direction in Wirld space
	 * bxdfResult: BSDF value 
	 * bxdfType: type of the sampled BxDF
	 */
	virtual void sample(Vec const& color, Vec const& Wo, Vec& Wi, Vec& bxdfResult, float& pdfResult, int &bxdfType) const = 0;
	/*
	 * eval: evaluates the BSDF Woth given incoming and outgoing directions
	 * color: sampled texture color
	 * Wo: incoming direction in Wirld space
	 * Wi: outgoing direction in Wirld space
	 * bxdfResult: BSDF value 
	 */
	virtual void eval(Vec const& color, Vec const& Wo, Vec const& Wi, Vec& bxdfResult, float &pdfResult) const = 0;
	//virtual float weight(Vec const& Wo) const = 0;

	static inline float cosTheta(Vec const& W) {
		return W.z();
	}

private:
};

static inline float fresnelCoef(
	Vec const& in, 
	float R0)
{
	float c = 1 - in.z();
	return R0 + (1-R0)*c*c*c*c*c;
}

//===================================
// Mirrors
class MirrorBRDF : public BxDF
{
public:
	MirrorBRDF(float R0) : m_R0(R0) {}
	~MirrorBRDF() {}

	virtual void sample(Vec const& color,
						Vec const& Wo, 
						Vec& Wi, 
						Vec& bxdfResult,
						float& pdfResult,
						int& bxdfType) const 
	{
		pdfResult = 1.f;	// should be a dirac?
		Wi = perfectSpecularReflection(Wo);
		bxdfResult = color * fresnelCoef(Wi, m_R0);
	}

	virtual void eval(Vec const& color,
					  Vec const& Wo, 
					  Vec const& Wi, 
					  Vec& bxdfResult,
					  float& pdfResult) const 
	{
		bxdfResult = Vec(0.f, 0.f, 0.f);
		pdfResult = 0.f;
	}

private:
	float m_R0;
};

//===================================
// Lambertian diffuse reflection
class LambertianBRDF : public BxDF
{
public:
	LambertianBRDF() {} 
	~LambertianBRDF() {}

	virtual void sample(Vec const& color,
		Vec const& Wo, 
		Vec& Wi, 
		Vec& bxdfResult,
		float& pdfResult,
		int& bxdfType) const 
	{
		pdfResult = 1.f;
		Wi = cosineSampleHemisphere();
		bxdfResult = color * 1.f / M_PI;
	}

	virtual void eval(Vec const& color,
		Vec const& Wo, 
		Vec const& Wi, 
		Vec& bxdfResult,
		float& pdfResult) const 
	{
		bxdfResult = color * cosTheta(Wi) * cosTheta(Wo);
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
		m_phongExp(phongExp), 
		m_highlightsColor(highlightsColor) 
	{} 
	~PhongBRDF() {}

	virtual void sample(Vec const& color,
		Vec const& Wo, 
		Vec& Wi, 
		Vec& bxdfResult,
		float& pdfResult,
		int& bxdfType) const 
	{
		Vec R = perfectSpecularReflection(Wo);
		Vec T, S;
		genOrtho(R, T, S);
		Vec Rg = cosinePowerSampleHemisphere(m_phongExp);
		Wi = Rg.x() * T + Rg.y() * S + Rg.z() * R;
		if (Wi.z() < 0) {
			Wi = - Rg.x() * T - Rg.y() * S + Rg.z() * R;
		}
		
		bxdfResult = m_highlightsColor * /*powf(dot(R, Wi), m_phongExp) **/ fresnelCoef(Wi, 0.5f);
		bxdfType = BxDF_REFLECTION | BxDF_GLOSSY;
		// TODO PDFs
		pdfResult = 1.f;
	}

	virtual void eval(Vec const& color,
		Vec const& Wo, 
		Vec const& Wi, 
		Vec& bxdfResult,
		float& pdfResult) const 
	{
		Vec H = halfway(Wo, Wi);
		pdfResult = 1.f;
		bxdfResult = color * m_highlightsColor * powf(H.z(), m_phongExp) * cosTheta(Wo) * cosTheta(Wi);
	}

private:
	float m_phongExp;
	Vec m_highlightsColor;
};



//===================================
// MixBSDF
/*
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
						Vec const& Wo, 
						Vec& Wi, 
						Vec& bxdfResult, 
						float& pdfResult, 
						BxDFType& bxdfType) const 
	{

		if (bxdfId != -1) {
			std::cerr << "MixBSDF::sample: unimplemented\n";
			return;
		}
		bxdfId = simulateCdf(m_numBxdfs, m_cdf, m_cdf[m_numBxdfs-1]);
		m_bxdfs[bxdfId]->sample(color, Wo, Wi, bxdfResult, pdfResult);
		bxdfType = m_bxdfs[bxdfId]->type;
	}

private:
	float m_weights[MAX_BxDFs];
	float m_cdf[MAX_BxDFs];
	BxDF const *m_bxdfs[MAX_BxDFs];
	int m_numBxdfs;
};
*/
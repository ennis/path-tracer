#pragma once
#include "ray.hpp"
#include "util.hpp"
#include "vec.hpp"
#include "primitive.hpp"
#include <iostream>

static const unsigned int MAX_BSDF_SAMPLES = 4;

enum BxDFType
{
	BxDF_REFLECTION = (1 << 0),
	BxDF_TRANSMISSION = (1 << 1),
	BxDF_DIFFUSE = (1 << 2),
	BxDF_SPECULAR = (1 << 3),
	BxDF_GLOSSY = (1 << 4)
};


// TODO Unified material class
class Material
{
public:
	
	virtual void init(Intersection &isect) const {
	}

	/*
	 * sample
	 * bxdfId: BxDF to sample (0..numComponents-1) or -1 to choose at random
	 * color: sampled texture color
	 * N: surface normal in World coordinates
	 * Wo: incoming direction in World space
	 * Wi: sampled direction in World space
	 * bxdfResult: BSDF value 
	 * bxdfType: type of the sampled BxDF
	 */
	virtual Vec sample(Intersection const &isect, float u1, float u2, Vec& WiW, float& pdfResult, int &bxdfType) const = 0;

	/*
	 * eval: evaluates the BSDF Woth given incoming and outgoing directions
	 * color: sampled texture color
	 * Wo: incoming direction in World space
	 * Wi: outgoing direction in World space
	 * bxdfResult: BSDF value 
	 */
	virtual Vec eval(Intersection const &isect, Vec const& WiW) const = 0;

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
class MirrorMaterial : public Material
{
public:
	MirrorMaterial(float R0, Texture const *texture) : m_R0(R0), m_texture(texture)
	{}

	~MirrorMaterial() 
	{}

	virtual void init(Intersection &isect) const
	{
		isect.texSample = m_texture->sample(isect.u, isect.v);
	}

	virtual Vec sample(
		Intersection const &isect,
		float u1,
		float u2,
		Vec& WiW, 
		float& pdfResult,
		int &bxdfType) const 
	{
		pdfResult = 1.f;	// should be a dirac?
		Vec Wi = perfectSpecularReflection(isect.toLocal(isect.WoW));
		WiW = isect.toWorld(Wi);
		bxdfType = BxDF_SPECULAR;
		return isect.texSample * fresnelCoef(Wi, m_R0);
	}

	virtual Vec eval(Intersection const &isect, Vec const& WiW) const 
	{
		return Vec(0.f, 0.f, 0.f);
	}

private:
	float m_R0;
	Texture const *m_texture;
};


//===================================
// Lambertian diffuse reflection
class LambertianMaterial : public Material
{
public:
	LambertianMaterial(Texture const *albedo) : m_albedo(albedo) {} 
	~LambertianMaterial() {}

	virtual void init(Intersection &isect) const
	{
		isect.texSample = m_albedo->sample(isect.u, isect.v);
	}

	virtual Vec sample(
		Intersection const &isect,
		float u1, float u2, 
		Vec& WiW, 
		float& pdfResult, 
		int &bxdfType) const 
	{
		pdfResult = 1.f;
		WiW = isect.toWorld(cosineSampleHemisphere(u1, u2));
		bxdfType = BxDF_DIFFUSE;
		return isect.texSample;
	}

	virtual Vec eval(
		Intersection const &isect, 
		Vec const& WiW) const 
	{
		
		return isect.texSample / M_PI;
	}

private:
	Texture const *m_albedo;
};

//===================================
// Phong specular highlights
/*class PhongBRDF : public BxDF
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
		
		bxdfResult = m_highlightsColor * powf(dot(R, Wi), m_phongExp) * fresnelCoef(Wi, 0.5f);
		bxdfType = BxDF_REFLECTION | BxDF_GLOSSY;
		// TODO PDFs
		pdfResult = 1.f;
	}

	virtual void eval(Vec const& color,
		Vec const& Wo, 
		Vec const& Wi, 
		Vec& bxdfResult) const 
	{
		Vec R = perfectSpecularReflection(Wo);
		bxdfResult = m_highlightsColor * powf(dot(R, Wi), m_phongExp) * cosTheta(Wi);
	}

private:
	float m_phongExp;
	Vec m_highlightsColor;
};*/



/*class AshikhminShirleyBRDF : public BxDF
{
public:
	AshikhminShirleyBRDF(float rd, float rs, float nu, float nv) : m_rd(rd), m_rs(rs), m_nu(nu), m_nv(nv)
	{}

	virtual void sample(
		Vec const& color,
		Vec const& Wo, 
		Vec& Wi, 
		Vec& bxdfResult,
		float& pdfResult,
		int& bxdfType) const
	{
		sampleHemisphere(Wo, Wi, pdfResult);
		if (pdfResult == 0.f || Wi.z() < 0.f) {
			bxdfType = BxDF_GLOSSY;
			bxdfResult = Vec(0.f, 0.f, 0.f);
		}
		else {
			bxdfType = BxDF_GLOSSY;
			eval(color, Wo, Wi, bxdfResult);
			bxdfResult *= cosTheta(Wi) / pdfResult;
		}	
	}

	virtual void eval(
		Vec const& color,
		Vec const& Wo, 
		Vec const& Wi, 
		Vec& bxdfResult) const
	{
		Vec H = halfway(Wi, Wo);
		
		float LdotN = Wi.z();
		float VdotN = Wo.z();
		float HdotU = H.x();
		float HdotV = H.y();
		float HdotN = H.z();
		float HdotL = dot(H,Wi);

		// diffuse term
		float pd = m_rd;
		pd *= (1.0f - m_rs);
		pd *= (1.0f - pow(1.0f - (LdotN / 2.0f), 5.0f));
		pd *= (1.0f - pow(1.0f - (VdotN / 2.0f), 5.0f));
		

		// specular term
		float ps_num_exp = m_nu * HdotU * HdotU + m_nv * HdotV * HdotV;
		ps_num_exp /= (1.0f - HdotN * HdotN);
 
		float ps_num = sqrt( (m_nu + 1) * (m_nv + 1) );
		ps_num *= pow( HdotN, ps_num_exp );
 
		float ps_den = 8.0f * M_PI * HdotL;
		ps_den *= std::max( LdotN, VdotN );
 
		float ps = m_rs * (ps_num / ps_den);
		ps *= ( m_rs + (1.0f - m_rs) * pow( 1.0f - HdotL, 5.0f ) );

		//std::cout << pd <<'\n';

		float p = ps + pd;
		bxdfResult = color * p;
	}

	inline void sampleHemisphere(Vec const &Wo, Vec &Wi, float &pdf) const
	{
		float x1 = frand(0.f, 1.f);
		float x2 = frand(0.f, 1.f);
		float cost, phi, exp;

		if (0.f < x1 && x1 <= 0.25f) {
			sampleQuadrant(4.f * x1, x2, phi, cost, exp);
		} else if (0.25f < x1 && x1 <= 0.5f) {
			sampleQuadrant(4.f * (0.5f - x1), x2, phi, cost, exp);
			phi = M_PI - phi;
		} else if (0.5f < x1 && x1 <= 0.75f) {
			sampleQuadrant(4.f * (0.75f - x1), x2, phi, cost, exp);
			phi += M_PI;
		} else {
			sampleQuadrant(4.f * (1.f - x1), x2, phi, cost, exp);
			phi = 2.f*M_PI - phi;
		}

		float sint2 = 1 - cost * cost;
		float sint = sqrtf(sint2);

		Vec H = Vec(sint * cosf(phi), sint * sinf(phi), cost);
		if (H.z() < 0.f) {
			H = -H;
		}

		Wi = -Wo + 2.f * dot(Wo, H) * H;
		if (Wi.z() < 0.f) {
			//std::clog << "shit\n";
		}

		float WodotH = dot(Wo, H);

		if (WodotH > 0.f) {
			float ph = sqrtf((m_nu + 1.f) * (m_nv + 1.f)) / (2.f * M_PI) * powf(H.z(), exp); 
			//std::clog << ph << "\n";
			pdf = ph / (4.f * dot(Wo, H));
		} else {
			pdf = 0.f;
		}
		//std::clog << pdf << "\n";
	}

	inline void sampleQuadrant(float x1, float x2, float &phi, float &cost, float &exp) const 
	{
		phi = atanf(sqrtf((m_nu + 1.f) / (m_nv + 1.f)) * tanf(M_PI * x1 / 2.f));
		float cosphi = cosf(phi), sinphi = sinf(phi);
		exp = m_nu * cosphi * cosphi + m_nv * sinphi * sinphi;
		cost = powf(x2, 1.f / (exp + 1.f));
	}

private:
	float m_rd, m_rs, m_nu, m_nv;
};*/

//===================================
// Glass
class GlassMaterial : public Material
{
public:
	GlassMaterial(float ior) :
		m_ior(ior)
	{
		m_r0 = (1-m_ior)/(1+m_ior);
		m_r0 *= m_r0;
	} 

	virtual ~GlassMaterial() 
	{}

	virtual Vec sample(
		Intersection const &isect, 
		float u1, float u2, 
		Vec& WiW, 
		float& pdfResult,
		int &bxdfType) const 
	{
		Vec WoL = isect.toLocal(isect.WoW);
		Vec result;
		if (WoL.z() < 0.f) {
			Vec Wi = perfectSpecularRefraction(-WoL, 1.f / m_ior);
			result = Vec(1.f, 1.f, 1.f) * fresnelCoef(Wi * Vec(1.f, 1.f, -1.f), m_r0);
			WiW = isect.toWorld(-Wi);
		} else {
			Vec Wi = perfectSpecularRefraction(WoL, m_ior);
			result = Vec(1.f, 1.f, 1.f) * fresnelCoef(Wi * Vec(1.f, 1.f, -1.f), m_r0);
			WiW = isect.toWorld(Wi);
		}
		bxdfType = BxDF_SPECULAR;
		pdfResult = 1.f;
		return result;
	}

	virtual Vec eval(
		Intersection const &isect, 
		Vec const& WiW) const 
	{
		return Vec(0.f, 0.f, 0.f);
	}

private:
	float m_ior;
	float m_r0;
};


//===================================
// BRDF weighted mix
/*class MixBRDF : public BxDF
{
public:
	MixBRDF(BxDF const *bxdf1, BxDF const *bxdf2, float weight) :
		m_bxdf1(bxdf1),
		m_bxdf2(bxdf2),
		m_weight(weight)
	{} 

	virtual ~MixBRDF() 
	{}

	virtual void sample(Vec const& color,
		Vec const& Wo, 
		Vec& Wi, 
		Vec& bxdfResult,
		float& pdfResult,
		int& bxdfType) const 
	{
		float x = frand(0.f, 1.f);
		if (x < m_weight) {
			m_bxdf1->sample(color, Wo, Wi, bxdfResult, pdfResult, bxdfType);
		} else {
			m_bxdf2->sample(color, Wo, Wi, bxdfResult, pdfResult, bxdfType);
		}
	}

	virtual void eval(Vec const& color,
		Vec const& Wo, 
		Vec const& Wi, 
		Vec& bxdfResult) const 
	{
		// weighted average
		Vec L1, L2;
		m_bxdf1->eval(color, Wo, Wi, L1);
		m_bxdf2->eval(color, Wo, Wi, L2);
		bxdfResult = m_weight * L1 + (1.f - m_weight) * L2;
	}

private:
	BxDF const *m_bxdf1, *m_bxdf2;
	float m_weight;
};*/

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
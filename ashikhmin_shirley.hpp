#pragma once
#include "bsdf.hpp"
#include "ray.hpp"
#include <cmath>
#include <algorithm>

// http://www.cs.utah.edu/~michael/brdfs/jgtbrdf.pdf

class AshikhminShirleyBSDF : public BSDF
{
public:
	AshikhminShirleyBSDF(float rd, float rs, float nu, float nv) : m_rd(rd), m_rs(rs), m_nu(nu), m_nv(nv)
	{}

	Vec sample(Vec const& N, Vec const& in, Vec const& color, float sampleX, float sampleY, Vec& out, bool& specular) const
	{
		out = sampleCosineWeightedRay(N, sampleX, sampleY);
		specular = false;
		return eval(N, in, out, color);
	}

	Vec eval(Vec const& N, Vec const& in, Vec const& out, Vec const& color) const
	{
		Vec U, V, H;
		genOrtho(N, U, V);
		H = halfway(in, out);
		
		float LdotN = dot(N,out);
		float VdotN = dot(N,in);
		float HdotU = dot(H,U);
		float HdotV = dot(H,V);
		float HdotN = dot(H,N);
		float HdotL = dot(H,out);

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
		return Vec(p,p,p);
	}

private:
	 float m_rd, m_rs, m_nu, m_nv;
};

// Several options
// Renderer sample texture value at intersection point, pass value to BRDF

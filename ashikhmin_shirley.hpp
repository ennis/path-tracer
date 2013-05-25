#pragma once
#include "bsdf.hpp"
#include "ray.hpp"
#include <cmath>
#include <algorithm>

class AshikhminShirleyBSDF : public BSDF
{
public:
	AshikhminShirleyBSDF(float rd, float rs, float nu, float nv) : m_rd(rd), m_rs(rs), m_nu(nu), m_nv(nv)
	{}

	Vec sample(Vec const& in, Vec const& N, Vec& out, bool& specular, float sampleX, float sampleY) const
	{
		out = sampleCosineWeightedRay(N, sampleX, sampleY);
		specular = false;
		return eval(in, out, N);
	}

	Vec eval(Vec const& in, Vec const& out, Vec const& N) const
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
		float pd = (28.0f * m_rd) / ( 23.0f * 3.14159f );
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
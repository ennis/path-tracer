#pragma once
#include <nmmintrin.h>
#include <ostream>
#include <cmath>
#include "constants.hpp"

// TODO make something more serious
struct Vec
{
	/*Vec() : x(0), y(0), z(0) {}
	Vec (float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
	
	float norm() const {
		return sqrt(x*x + y*y + z*z);
	}

	Vec normalized() const {
		float n = norm();
		return Vec(x/n, y/n, z/n);
	}*/
	
	Vec()
	{
		xyzw = _mm_setzero_ps();
	}

	Vec(float x, float y, float z, float w = 0.0f) 
	{
		xyzw = _mm_setr_ps(x, y, z, w);
	}

	Vec(__m128 init) : xyzw(init)
	{}

	float norm() const {
		__m128 s = _mm_dp_ps(xyzw, xyzw, 0xF1);
		s = _mm_sqrt_ss(s);
		return _mm_cvtss_f32(s);
	}

	Vec normalized() const {
		float s = _mm_cvtss_f32(_mm_dp_ps(xyzw, xyzw, 0xF1));
	if (fabs(s - 1.0f) < EPSILON) {
		return *this;
	}
	// estimate + NR
	float half = 0.5f * s;
		s = _mm_cvtss_f32(_mm_rsqrt_ss(_mm_set_ss(s)));
	s = s * (1.5f - half*s*s);
	return Vec(_mm_mul_ps(xyzw, _mm_set1_ps(s)));
	}

	float x() const {
		// dumb api is dumb
		float f;
		_MM_EXTRACT_FLOAT(f, xyzw, 0);
		return f;
	}
	
	float y() const {
		float f;
		_MM_EXTRACT_FLOAT(f, xyzw, 1);
		return f;
	}

	float z() const {
		float f;
		_MM_EXTRACT_FLOAT(f, xyzw, 2);
		return f;
	}

	float w() const {
		float f;
		_MM_EXTRACT_FLOAT(f, xyzw, 3);
		return f;
	}

	__m128 xyzw;
};

static const Vec nullVec = Vec();

struct Point : public Vec
{
	Point() : Vec(0, 0, 0, 1)
	{}
	
	Point(__m128 init) : Vec(init)
	{}
	
	Point(float x, float y, float z) : Vec(x, y, z, 1)
	{}

};

static const Vec COLOR_BLACK = Vec(0,0,0);

/*Vec operator + (Vec const& v1, Vec const& v2) { return Vec(v1.x+v2.x, v1.y+v2.y, v1.z+v2.z); }
Vec operator - (Vec const& v1, Vec const& v2) { return Vec(v1.x-v2.x, v1.y-v2.y, v1.z-v2.z); }
Vec operator * (float s, Vec const& v) { return Vec(s*v.x, s*v.y, s*v.z); }
Vec operator * (Vec const& v, float s) { return Vec(s*v.x, s*v.y, s*v.z); }
Vec operator * (Vec const& u, Vec const& v) { return Vec(u.x*v.x, u.y*v.y, u.z*v.z); }  // component multiply
float dot (Vec const& u, Vec const& v) { return u.x*v.x + u.y*v.y + u.z*v.z; }
Vec cross (Vec const& u, Vec const& v) { return Vec(u.y*v.z-u.z*v.y, v.x*u.z-v.z*u.x, u.x*v.y-u.y*v.x); }*/

static inline Vec operator + (Vec const& V1, Vec const& V2) { return Vec(_mm_add_ps(V1.xyzw, V2.xyzw)); }
static inline Vec operator - (Vec const& V1, Vec const& V2) { return Vec(_mm_sub_ps(V1.xyzw, V2.xyzw)); }

static inline Point operator + (Point const& V1, Vec const& V2) { return Point(_mm_add_ps(V1.xyzw, V2.xyzw)); }
static inline Point operator - (Point const& V1, Vec const& V2) { return Point(_mm_sub_ps(V1.xyzw, V2.xyzw)); }

static inline Vec operator-(Vec const& v1) { 
	static const __m128 SIGNMASK = _mm_castsi128_ps(_mm_set1_epi32(0x80000000));
	return Vec(_mm_xor_ps(v1.xyzw, SIGNMASK));
}

static inline void operator+=(Vec& v1, Vec const& v2)
{
	v1.xyzw = _mm_add_ps(v1.xyzw, v2.xyzw);
}

static inline void operator-=(Vec& v1, Vec const& v2)
{
	v1.xyzw = _mm_sub_ps(v1.xyzw, v2.xyzw);
}

static inline void operator*=(Vec& v1, Vec const& v2)
{
	v1.xyzw = _mm_sub_ps(v1.xyzw, v2.xyzw);
}

static inline Vec operator * (float s, Vec const& V) 
{
	__m128 t = _mm_set1_ps(s);
	return Vec(_mm_mul_ps(t,V.xyzw));
}

static inline Vec operator * (Vec const& V, float s) 
{ 
	return s * V;
}

static inline void operator *= (Vec & V, float s) 
{ 
	V = s * V;
}

static inline Vec operator / (Vec const& V, float s) 
{ 
	__m128 t = _mm_set1_ps(s);
	return Vec(_mm_div_ps(V.xyzw,t));
}

// component multiply
static inline Vec operator * (Vec const& U, Vec const& V) { 
	return Vec(_mm_mul_ps(U.xyzw,V.xyzw)); 
}

static inline float dot (Vec const& U, Vec const& V) { 
	return _mm_cvtss_f32(_mm_dp_ps(U.xyzw, V.xyzw, 0xFF)); 
}

static inline Vec cross (Vec const& U, Vec const& V) {
	return Vec(_mm_sub_ps(
				_mm_mul_ps(_mm_shuffle_ps(U.xyzw, U.xyzw, _MM_SHUFFLE(3, 0, 2, 1)), _mm_shuffle_ps(V.xyzw, V.xyzw, _MM_SHUFFLE(3, 1, 0, 2))),
				_mm_mul_ps(_mm_shuffle_ps(U.xyzw, U.xyzw, _MM_SHUFFLE(3, 1, 0, 2)), _mm_shuffle_ps(V.xyzw, V.xyzw, _MM_SHUFFLE(3, 0, 2, 1))))); 
}

static inline Vec clamp(Vec const& V, float low, float high)
{
	__m128 Vl = _mm_set1_ps(low);
	__m128 Vh = _mm_set1_ps(high);
	return (_mm_max_ps(_mm_min_ps(V.xyzw,Vh),Vl));
}

static inline bool parallel(Vec const& u, Vec const& v) 
{
	return cross(u, v).norm() < EPSILON;
}

static inline bool operator==(Vec const& u, Vec const& v)
{
	return u.x() == v.x() && u.y() == v.y() && u.z() == v.z();
}

static inline void genOrtho(Vec const& N, Vec& T, Vec& S)
{
	if (!parallel(N, Vec(0,0,1))) 
		T = cross(N, Vec(0,0,1)).normalized();
	else 
		T = cross(N, Vec(0,1,0)).normalized();
	S = cross(N,T).normalized();
}

static inline std::ostream& operator<< (std::ostream& os, Vec const& v)
{
	os << '<' << v.x() << ' ' << v.y() << ' ' << v.z() << ' ' << v.w() << '>';
	return os;
}
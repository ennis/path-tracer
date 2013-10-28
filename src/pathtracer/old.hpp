#pragma once
#include <cstring>
#include "vec.hpp"
#include "ray.hpp"

struct Matrix4x4
{
	Matrix4x4() 
	{
		d[0][0] = 1; d[0][1] = 0; d[0][2] = 0; d[0][3] = 0;
		d[1][0] = 0; d[1][1] = 1; d[1][2] = 0; d[1][3] = 0;
		d[2][0] = 0; d[2][1] = 0; d[2][2] = 1; d[2][3] = 0;
		d[3][0] = 0; d[3][1] = 0; d[3][2] = 0; d[3][3] = 1;
	}
	
	Matrix4x4(float init[4][4])
	{
		memcpy(d, init, 4*4*sizeof(float));
	}

	Matrix4x4 (float m00, float m01, float m02, float m03,
				float m10, float m11, float m12, float m13,
				float m20, float m21, float m22, float m23,
				float m30, float m31, float m32, float m33) 
	{
		d[0][0] = m00; d[0][1] = m01; d[0][2] = m02; d[0][3] = m03;
		d[1][0] = m10; d[1][1] = m11; d[1][2] = m12; d[1][3] = m13;
		d[2][0] = m20; d[2][1] = m21; d[2][2] = m22; d[2][3] = m23;
		d[3][0] = m30; d[3][1] = m31; d[3][2] = m32; d[3][3] = m33;
	}

	Vec apply(Vec const& v) const 
	{
		return Vec(d[0][0]*v.x() + d[0][1]*v.y() + d[0][2]*v.z() + d[0][3]*v.w(),
				d[1][0]*v.x() + d[1][1]*v.y() + d[1][2]*v.z() + d[1][3]*v.w(),
				d[2][0]*v.x() + d[2][1]*v.y() + d[2][2]*v.z() + d[2][3]*v.w(),
				d[3][0]*v.x() + d[3][1]*v.y() + d[3][2]*v.z() + d[3][3]*v.w());
	}

	float d[4][4];

	bool inverse(Matrix4x4& result);
	Vec translation() const {
		return Vec(d[0][3], d[1][3], d[2][3]);
	}
	
	static Matrix4x4 lookAtLH(Point const& eye, Point const& lookat, Vec const& up);
	static Matrix4x4 lookAtRH(Point const& eye, Point const& lookat, Vec const& up);
	static Matrix4x4 translate(Vec const& vec);
	static Matrix4x4 scale(float f);

};

static const Matrix4x4 identityM = Matrix4x4();

Matrix4x4 operator*(Matrix4x4 const& m1, Matrix4x4 const& m2);

std::ostream& operator<< (std::ostream& os, Matrix4x4 const& m);

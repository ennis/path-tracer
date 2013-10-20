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


	float d[4][4];
};

Matrix4x4 MLookAtLH(Point const& eye, Point const& lookat, Vec const& up);
Matrix4x4 MLookAtRH(Point const& eye, Point const& lookat, Vec const& up);
Matrix4x4 MTranslate(Vec const& vec);
Matrix4x4 MScale(float f);
Matrix4x4 MScale(Vec const& v);
Matrix4x4 MTranspose(Matrix4x4 const& m);
bool MInverse(Matrix4x4 const& mat, Matrix4x4& result); 
Matrix4x4 MMultiply(Matrix4x4 const& m1, Matrix4x4 const& m2); 
// TODO
Matrix4x4 MMultiplyTranspose1(Matrix4x4 const& m1, Matrix4x4 const& m2); 
Matrix4x4 MMultiplyTranspose2(Matrix4x4 const& m1, Matrix4x4 const& m2); 

Vec MApply(Matrix4x4 const& mat, Vec const& v);
Vec MApplyTranspose(Matrix4x4 const& mat, Vec const& v);
Point MApplyPoint(Matrix4x4 const& mat, Point const& p);


Vec MGetTranslation(Matrix4x4 const& mat);

static const Matrix4x4 identityM = Matrix4x4();

static inline Matrix4x4 operator*(Matrix4x4 const& m1, Matrix4x4 const& m2) {
	return MMultiply(m1, m2);
}

std::ostream& operator<< (std::ostream& os, Matrix4x4 const& m);

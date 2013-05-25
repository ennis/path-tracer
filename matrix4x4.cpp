#include "matrix4x4.hpp"
#include <utility>

using namespace std;

// taken from pbrt core/transform.cpp
bool MInverse(Matrix4x4 const& mat, Matrix4x4 &result)
{
    int indxc[4], indxr[4];
    int ipiv[4] = { 0, 0, 0, 0 };
    float (&minv)[4][4] = result.d;
	memcpy(minv, mat.d, 4*4*sizeof(float));
    for (int i = 0; i < 4; i++) {
        int irow = -1, icol = -1;
        float big = 0.;
        // Choose pivot
        for (int j = 0; j < 4; j++) {
            if (ipiv[j] != 1) {
                for (int k = 0; k < 4; k++) {
                    if (ipiv[k] == 0) {
                        if (fabsf(minv[j][k]) >= big) {
                            big = float(fabsf(minv[j][k]));
                            irow = j;
                            icol = k;
                        }
                    }
                    else if (ipiv[k] > 1) {
                        //Error("Singular matrix in MatrixInvert");
						// singular matrix
                        return false;
					}
                }
            }
        }
        ++ipiv[icol];
        // Swap rows _irow_ and _icol_ for pivot
        if (irow != icol) {
            for (int k = 0; k < 4; ++k) {
                swap(minv[irow][k], minv[icol][k]);
			}
        }
        indxr[i] = irow;
        indxc[i] = icol;
        if (minv[icol][icol] == 0.) {
            //Error("Singular matrix in MatrixInvert");
            return false;
		}

        // Set $m[icol][icol]$ to one by scaling row _icol_ appropriately
        float pivinv = 1.f / minv[icol][icol];
        minv[icol][icol] = 1.f;
        for (int j = 0; j < 4; j++)
            minv[icol][j] *= pivinv;

        // Subtract this row from others to zero out their columns
        for (int j = 0; j < 4; j++) {
            if (j != icol) {
                float save = minv[j][icol];
                minv[j][icol] = 0;
                for (int k = 0; k < 4; k++)
                    minv[j][k] -= minv[icol][k]*save;
            }
        }
    }
    // Swap columns to reflect permutation
    for (int j = 3; j >= 0; j--) {
        if (indxr[j] != indxc[j]) {
            for (int k = 0; k < 4; k++)
                swap(minv[k][indxr[j]], minv[k][indxc[j]]);
        }
    }
    
    return true;
}

Matrix4x4 MMultiply(Matrix4x4 const& m1, Matrix4x4 const& m2);
{
	// TODO?
	return Matrix4x4();
}

Matrix4x4 MLookAtLH(Point const& eye, Point const& lookat, Vec const& up)
{
	// camera space to world space
	Vec Z = (lookat - eye).normalized();
	Vec X = cross(up.normalized(), Z).normalized();
	Vec Y = cross(Z, X);

	return Matrix4x4(X.x(), Y.x(), Z.x(), -dot(X,eye),
			        X.y(), Y.y(), Z.y(), -dot(Y,eye),
					X.z(), Y.z(), Z.z(), -dot(Z,eye),
					0, 0, 0, 1);
}

Matrix4x4 MLookAtRH(Point const& eye, Point const& lookat, Vec const& up)
{
    // camera space to world space
    Vec Z = (lookat - eye).normalized();
    Vec X = cross(up, Z).normalized();
    Vec Y = cross(X, Z);

    return Matrix4x4(X.x(), Y.x(), Z.x(), -dot(X,eye),
                    X.y(), Y.y(), Z.y(), -dot(Y,eye),
                    X.z(), Y.z(), Z.z(), -dot(Z,eye),
                    0, 0, 0, 1);
}

Matrix4x4 MTranslate(Vec const& vec)
{
	return Matrix4x4(1, 0, 0, vec.x(),
                     0, 1, 0, vec.y(), 
    				 0, 0, 1, vec.z(),
    				 0, 0, 0, 1);
}

Vec MApply(Matrix4x4 const& mat, Vec const& v) {
	return Vec(mat.d[0][0]*v.x() + mat.d[0][1]*v.y() + mat.d[0][2]*v.z() + mat.d[0][3]*v.w(),
			mat.d[1][0]*v.x() + mat.d[1][1]*v.y() + mat.d[1][2]*v.z() + mat.d[1][3]*v.w(),
			mat.d[2][0]*v.x() + mat.d[2][1]*v.y() + mat.d[2][2]*v.z() + mat.d[2][3]*v.w(),
			mat.d[3][0]*v.x() + mat.d[3][1]*v.y() + mat.d[3][2]*v.z() + mat.d[3][3]*v.w());
}

Vec MGetTranslation(Matrix4x4 const& mat) {
	return Vec(mat.d[0][3], mat.d[1][3], mat.d[2][3]);
}
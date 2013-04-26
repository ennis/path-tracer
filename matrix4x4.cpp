#include "matrix4x4.hpp"
#include <utility>

using namespace std;

// taken from pbrt core/transform.cpp
bool Matrix4x4::inverse(Matrix4x4 &result)
{
    int indxc[4], indxr[4];
    int ipiv[4] = { 0, 0, 0, 0 };
    float (&minv)[4][4] = result.d;
    memcpy(minv, d, 4*4*sizeof(float));
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

Matrix4x4 Matrix4x4::lookAtLH(Point const& eye, Point const& lookat, Vec const& up)
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

Matrix4x4 Matrix4x4::lookAtRH(Point const& eye, Point const& lookat, Vec const& up)
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

Matrix4x4 Matrix4x4::translate(Vec const& vec)
{
	return Matrix4x4(1, 0, 0, vec.x(),
                     0, 1, 0, vec.y(), 
    				 0, 0, 1, vec.z(),
    				 0, 0, 0, 1);
}

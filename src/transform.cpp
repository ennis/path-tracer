#include "transform.hpp"
#include "matrix4x4.hpp"


Vec transformVec(Transform const& T, Vec const& v) {
	return MApply(T.m_t, v);
}

Vec invTransformVec(Transform const& T, Vec const& v) {
	return MApply(T.m_inv, v);
}

Ray transformRay(Transform const& T, Ray const& r) {
	return Ray(transformPoint(T, r.O), transformVec(T, r.D));
}

Ray invTransformRay(Transform const& T, Ray const& r) {
	return Ray(invTransformPoint(T, r.O), invTransformVec(T, r.D));
}

Point transformPoint(Transform const& T, Point const& p) {
	return MApplyPoint(T.m_t, p);
}

Point invTransformPoint(Transform const& T, Point const& p) {
	return MApplyPoint(T.m_inv, p);
}

Vec transformNormal(Transform const& T, Vec const& n) {
	return MApplyTranspose(T.m_inv, n);
}

Vec invTransformNormal(Transform const& T, Vec const& n) {
	return MApplyTranspose(T.m_t, n);
}
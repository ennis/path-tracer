#include "transform.hpp"
#include "matrix4x4.hpp"


Vec transform(Transform const& T, Vec const& v) {
	return MApply(T.m_t, v);
}

Vec invTransform(Transform const& T, Vec const& v) {
	return MApply(T.m_inv, v);
}

Ray transform(Transform const& T, Ray const& r) {
	return Ray(transform(T, r.O), transform(T, r.D));
}

Ray invTransform(Transform const& T, Ray const& r) {
	return Ray(invTransform(T, r.O), invTransform(T, r.D));
}

Point transform(Transform const& T, Point const& p) {
	return MApplyPoint(T.m_t, p);
}

Point invTransform(Transform const& T, Point const& p) {
	return MApplyPoint(T.m_inv, p);
}

Vec transformNormal(Transform const& T, Vec const& n) {
	return MApplyTranspose(T.m_inv, n);
}

Vec invTransformNormal(Transform const& T, Vec const& n) {
	return MApplyTranspose(T.m_t, n);
}
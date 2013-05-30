#pragma once
#include "matrix4x4.hpp"
#include "ray.hpp"

struct Transform
{
	Transform() : m_t(identityM), m_inv(identityM)
	{}

	Transform(Matrix4x4 const& t) : m_t(t) 
	{
		MInverse(m_t, m_inv);
	}

	Transform(Matrix4x4 const& t, Matrix4x4 const& inv) : m_t(t), m_inv(inv)
	{}

	Matrix4x4 m_t, m_inv;
};

static const Transform identityT = Transform();

Vec transformVec(Transform const& T, Vec const& v);
Vec invTransformVec(Transform const& T, Vec const& v);
Ray transformRay(Transform const& T, Ray const& r);
Ray invTransformRay(Transform const& T, Ray const& r);
Point transformPoint(Transform const& T, Point const& p);
Point invTransformPoint(Transform const& T, Point const& p);
Vec transformNormal(Transform const& T, Vec const& n);
Vec invTransformNormal(Transform const& T, Vec const& n);

static inline Transform operator*(Transform const& t1, Transform const& t2) 
{
	return Transform(t1.m_t*t2.m_t, t2.m_inv*t1.m_inv);
}

static inline Transform translate(Vec const& vec)
{
	return Transform(MTranslate(-vec), MTranslate(vec));
}

static inline Transform scale(Vec const& vec)
{
	Vec inv = Vec(1.f/vec.x(), 1.f/vec.y(), 1.f/vec.z());
	return Transform(MScale(inv), MScale(vec));
}
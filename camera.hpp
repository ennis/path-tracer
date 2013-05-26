#pragma once
#include "vec.hpp"
#include "matrix4x4.hpp"
#include "ray.hpp"

class Camera 
{
public:
	Camera(Point const& eye, Point const& lookAt, float width, float height, float screenDist = 1.f, Vec const& up = Vec(0,1,0)) : 
		m_eye(eye),
		m_lookAt(lookAt),
		m_up(up),
		m_width(width),
		m_height(height),
		m_screenDist(screenDist)
	{
		m_viewM = MLookAtRH(eye, lookAt, up);
		MInverse(m_viewM, m_invViewM);
	}

	Point const& getEye() const {
		return m_eye;
	}

	Point const& getLookAt() const {
		return m_lookAt;
	}

	Vec const& getUpVec() const {
		return m_up;
	}

	float getWidth() const {
		return m_width;
	}

	float getHeight() const {
		return m_height;
	}

	Ray rayThroughCameraPixel(float px, float py, float pixelWidth, float pixelHeight) const
	{
		Vec camdir((px / pixelWidth - 0.5f) * m_width, 
				   (py / pixelHeight - 0.5f) * m_height,
				   m_screenDist);
		return Ray(m_eye, MApply(m_viewM, camdir));
	}


private:
	Point m_eye;
	Point m_lookAt;
	Vec m_up;
	float m_width;
	float m_height;
	float m_screenDist;
	Matrix4x4 m_viewM;
	Matrix4x4 m_invViewM;
};

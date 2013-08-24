#pragma once
#include "vec.hpp"
#include "matrix4x4.hpp"
#include "ray.hpp"
#include "util.hpp"

struct Camera 
{
public:	
	Camera(Point const& eye, 
			Point const& lookAt, 
			float width, 
			float height, 
			float screenDist = 1.f,
			float apertureSize = 0.5f,
			Vec const& up = Vec(0,1,0)) : 
		m_eye(eye),
		m_lookAt(lookAt),
		m_up(up),
		m_width(width),
		m_height(height),
		m_screenDist(screenDist),
		m_apertureSize(apertureSize)
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
		/*Vec camdir((px / pixelWidth - 0.5f) * m_width, 
				   (py / pixelHeight - 0.5f) * m_height,
				   m_screenDist);*/

		Vec D = (m_lookAt - m_eye).normalized(), T, S;
		T = cross(m_up, D).normalized();
		S = cross(T, D).normalized();


		// circular aperture
		float r = m_apertureSize*sqrt(frand(0,1));
		float phi = frand(0, 2.0f*M_PI);

		float xp = frand(-m_apertureSize / 2.f, m_apertureSize / 2.f);
		float yp = frand(-m_apertureSize / 2.f, m_apertureSize / 2.f);

		Point pixel = m_lookAt + ((px / pixelWidth - 0.5f) * m_width) * T + ((py / pixelHeight - 0.5f) * m_height) * S;
		Point neye = m_eye + xp * T + yp * S;
		return Ray(neye, pixel - neye);
	}


private:
	Point m_eye;
	Point m_lookAt;
	Vec m_up;
	float m_width;
	float m_height;
	float m_screenDist;
	float m_apertureSize;
	Matrix4x4 m_viewM;
	Matrix4x4 m_invViewM;
};

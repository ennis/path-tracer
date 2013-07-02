#ifndef SCENE_HPP
#define SCENE_HPP

#include <vector>
#include "primitive.hpp"
#include "vec.hpp"

//==================================
// Scene
class Scene
{
public:
	Scene() : 
		m_ambient(Vec()), 
		m_camera(NULL) 
	{}

	~Scene()
	{}

	void add(Primitive const* primitive) {
		m_primitives.push_back(primitive);
	}

	void remove(Primitive const* primitive) {
		m_primitives.remove(primitive);
	}

	bool findIntersection(Ray const& ray, Intersection *isect) {
		static const float INFINITE = 1e30f;
		Intersection tmp_isect;
		isect->dist = INFINITE;
		bool hit = false;
		for (std::vector<Primitive const*>::const_iterator p = 
				m_primitives->cbegin(); 
			p != m_primitives->cend(); ++p)
		{
			if ((*p)->intersect(ray, &tmp_isect)) {
				// confirmed hit
				if (hit == false || tmp_isect.dist < isect->dist) {
					hit = true;
					*isect = tmp_isect;
				}
			}
		}
		return hit;
	}

	void setAmbient(Vec const &color) {
		m_ambient = color;
	}

	Vec getAmbient() const {
		return m_ambient;
	}

	void setCamera(Camera const *camera) {
		m_camera = camera;
	}

	Camera const *getCamera() const {
		return m_camera;
	}

private:
	// Ambient color (TODO textures, envmaps, sun & sky models)
	Vec m_ambient;
	// Camera
	Camera const *m_camera;
	// TODO BVH
	std::vector<Primitive const*> m_primitives;
};

#endif
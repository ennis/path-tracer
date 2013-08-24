#ifndef SCENE_HPP
#define SCENE_HPP

#include <vector>
#include "primitive.hpp"
#include "vec.hpp"
#include "envmap.hpp"

//==================================
// Scene
class Scene
{
public:
	Scene() : 
		m_ambient(Vec()), 
		m_camera(NULL),
		m_envmap(NULL)
	{}

	~Scene()
	{}

	void add(Primitive const* primitive) {
		m_primitives.push_back(primitive);
	}

	void addEmitter(Emitter const* emitter) {
		m_emitters.push_back(emitter);
	}

	void remove(Primitive const* primitive) {
		// TODO
	}

	Vec evalAmbient(Vec const &dir) const {
		if (m_envmap) {
			return m_envmap->eval(dir);
		} else {
			return getAmbient();
		}
	}

	// TODO check maxt
	bool findIntersection(Ray const& ray, Intersection *isect, float maxt = 1e30f) const {
		Intersection tmp_isect;
		isect->t = maxt;
		bool hit = false;
		for (std::vector<Primitive const*>::const_iterator p = 
				m_primitives.cbegin(); 
			p != m_primitives.cend(); ++p)
		{
			if ((*p)->intersect(ray, tmp_isect) && tmp_isect.t < maxt) {
				// confirmed hit
				if (hit == false || tmp_isect.t < isect->t) {
					hit = true;
					*isect = tmp_isect;
				}
			}
		}
		isect->WoW = -ray.D;
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

	void setEnvironmentMap(EnvironmentMap const *envmap) {
		m_envmap = envmap;
	}

	EnvironmentMap const *getEnvironmentMap() const {
		return m_envmap;
	}

	std::vector<Primitive const *> const &getPrimitives() const {
		return m_primitives;
	}

	std::vector<Emitter const *> const &getEmitters() const {
		return m_emitters;
	}
	
	// TODO getEmitters

private:
	// Ambient color (TODO textures, envmaps, sun & sky models)
	Vec m_ambient;
	// Camera
	Camera const *m_camera;
	// TODO BVH
	std::vector<Primitive const *> m_primitives;
	std::vector<Emitter const *> m_emitters;
	// Envmap (optional)
	EnvironmentMap const *m_envmap;
};

#endif
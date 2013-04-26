#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

#include <vector>
#include "object.hpp"

class Scene
{
public:
	Scene() {}
	~Scene() {}

	void addObject(Object const* obj);
	void removeObject(Object const* obj);

	std::vector<Object*> const& getObjects() {
		return objects;
	} 

private:
	std::vector<Object const*> objects;
};

#endif
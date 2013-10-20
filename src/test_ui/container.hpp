#ifndef CONTAINER_HPP
#define CONTAINER_HPP

#include "element.hpp"

#include <vector>

namespace ui
{
	class Container : public Element
	{
	public:
		virtual void add(Element::Ptr element);
		virtual bool handleEvent(sf::Event const &event);
		std::vector<Element::Ptr> const &getChildren() const;

	protected:
		std::vector<Element::Ptr> m_children;
	};
}

#endif
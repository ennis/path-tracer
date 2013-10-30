#include "container.hpp"

namespace ui
{
	void Container::add(Element::Ptr element)
	{
		m_children.push_back(element);
		element->setParent(shared_from_this());
	}

	bool Container::handleEvent(sf::Event const &event)
	{
		bool handled = false;
		// yeah, call super is bad, smells and all that
		// but the other alternative is a multilevel template method pattern, so fuck this shit.
		switch (event.type) {
		case sf::Event::MouseMoved:
			{
				bool hit = hitTest(event.mouseMove.x, event.mouseMove.y);
				if (hit != testState(HOVER)) {
					if (hit == true) {
						setState(HOVER);
						onHoverEnter();
					} else {
						clearState(HOVER);
						onHoverLeave();
					}
				}
				if (hit == true) {
					// pass event to children if hit test is successful
					for (auto child : m_children) {
						child->handleEvent(event);
					}
				}
			}
			break;

		case sf::Event::MouseButtonPressed:
			if (hitTest(event.mouseButton.x, event.mouseButton.y)) {
				bool childHandled = false;
				// propagate click to children
				for (auto child : m_children) {
					if (child->handleEvent(event)) {
						childHandled = true;
						break;
					}
				}
				// none of the children handled the click, so the container handles it
				if (!childHandled) {
					onClick(event.mouseButton.x, event.mouseButton.y);
				}
			}
			break;

		case sf::Event::MouseButtonReleased:
			clearState(Element::ACTIVE);
			for (auto child : m_children) {
				child->handleEvent(event);
			}
			break;

		default:
			handled = false;
			break;
		}

		return handled;
	}
}
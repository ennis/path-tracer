#include <iostream>

#include "ui.hpp"
#include "element.hpp"

namespace ui
{

void UI::setEngine(Engine::Ptr engine)
{
	m_engine = engine;
}

void UI::render(sf::RenderTarget &renderTarget)
{
	m_rootPanel->render(renderTarget, *m_engine);
}

void UI::add(Element::Ptr element)
{
	std::clog << "UI::add\n";
	m_rootPanel = element;
	doLayout();
}

void UI::doLayout()
{
	if (m_rootPanel != nullptr) {
		Size desired = m_rootPanel->getDesiredSize(*m_engine);
		BoundingBox childBB;
		childBB.x = 0;
		childBB.y = 0;
		
		if (desired.width == Size::FILL) {
			childBB.width = m_width;
		} else {
			childBB.width = std::min(m_width, desired.width);
		}


		if (desired.height == Size::FILL) {
			childBB.height = m_height;
		} else {
			childBB.height = std::min(m_height, desired.height);
		}

		m_rootPanel->layout(*m_engine, childBB);
	}
}

void UI::setSize(int width, int height)
{
	std::clog << "UI::setSize " << width << " " << height << "\n";
	m_width = width;
	m_height = height;
	doLayout();
}


void UI::onEvent(sf::Event const &event)
{
	if (m_rootPanel != nullptr) {
		m_rootPanel->handleEvent(event);
	}
}


}
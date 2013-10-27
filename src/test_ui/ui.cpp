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
		Size fixed = m_rootPanel->getFixedSize();
		Size desired = m_rootPanel->getDesiredSize(*m_engine);
		int x = 0;
		int y = 0;
		int width, height;

		if (fixed.hasWidth()) {
			width = std::min(m_width, fixed.width);
		} else if (desired.hasWidth() && m_rootPanel->getHorizontalSizePolicy() == SizePolicy::Preferred) {
			width = std::min(m_width, desired.width);
		} else {
			width = m_width;
		}

		if (fixed.hasHeight()) {
			height = std::min(m_height, fixed.height);
		} else if (desired.hasHeight() && m_rootPanel->getVerticalSizePolicy() == SizePolicy::Preferred) {
			height = std::min(m_height, desired.height);
		} else {
			height = m_height;
		}

		m_rootPanel->layout(*m_engine, BoundingBox(x, y, width, height));
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
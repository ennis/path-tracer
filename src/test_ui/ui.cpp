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
		m_rootPanel->calculateRequisition();
		m_rootPanel->setAllocation(BoundingBox(0, 0, m_width, m_height));
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
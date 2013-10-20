#include "panel.hpp"
#include "engine.hpp"

#include <iostream>

namespace ui
{

Panel::Panel(Panel::private_key) : m_orientation(VERTICAL), m_spacing(NO_SPACING)
{
	std::clog << "Panel\n";
}

Panel::Panel(private_key, Orientation orientation, Spacing spacing) : m_orientation(orientation), m_spacing(spacing)
{
	std::clog << "Panel(Orientation, Spacing)\n";
}

Panel::~Panel()
{
}

void Panel::render(sf::RenderTarget &renderTarget, Engine &engine)
{
	//std::clog << "Panel::render\n";
	engine.drawPanelFrame(renderTarget, *this);
	for (auto child : m_children) {
		child->render(renderTarget, engine);
	}
}

/*void Panel::calculateRequisition()
{
	m_reqWidth = -1;
	m_reqHeight = -1;
}*/

void Panel::setOrientation(Orientation orientation)
{
	m_orientation = orientation;
}

void Panel::onSetAllocation()
{
	// update maximum widget height
	int maxChildSize;
	if (m_orientation == VERTICAL) {
		maxChildSize = m_bounds.height / static_cast<int>(m_children.size());
	} else {
		maxChildSize = m_bounds.width / static_cast<int>(m_children.size());
	}
	// update bounding boxes of child elements
	int bbx = m_bounds.x + m_paddingLeft;
	int bby = m_bounds.y + m_paddingTop;
	for (auto child : m_children) {
		BoundingBox childBB;
		int reqWidth, reqHeight;
		child->calculateRequisition();
		child->getRequisition(reqWidth, reqHeight);
		childBB.x = bbx;
		childBB.y = bby;
		if (m_orientation == VERTICAL) {
			childBB.width = m_bounds.width - m_paddingLeft - m_paddingRight;
			childBB.height = (reqHeight == -1) ? maxChildSize : reqHeight;
			bby += childBB.height;
		} else {
			childBB.width = (reqWidth == -1) ? maxChildSize : reqWidth;
			childBB.height = m_bounds.height - m_paddingBottom - m_paddingTop;
			bbx += childBB.width;
		}

		child->setAllocation(childBB);
	}
}

}
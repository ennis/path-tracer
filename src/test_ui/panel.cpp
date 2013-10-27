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
	setMargin(Margins(0, 0, 0, 0));
	setPadding(Margins(1, 1, 1, 1));
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

Size Panel::getDesiredSize(Engine &engine)
{
	return Size(-1, -1);
}

/*void Panel::calculateRequisition(Engine &engine)
{
	Margins margin, padding;
	engine.getPanelMargins(margin, padding);
	boxRequisition(margin, padding);
}*/

void Panel::doLayout(Engine &engine)
{
	Margins margin, padding;
	engine.getPanelMargins(margin, padding);

	int totalHeight = m_bounds.height;
	int remHeight = totalHeight;

	// 0 : remove padding and spacing
	remHeight -= padding.top + padding.bottom;
	// 2px spacing (XXX engine.getPanelItemSpacing)
	remHeight -= m_children.size() < 1 ? 0 : m_children.size() * 2;

	// pass 1 : assign vertical space to children with a fixed size
	for (auto child : m_children) {
		Size fixed = child->getFixedSize();
		if (fixed.hasHeight()) {
			remHeight -= fixed.height;
		}
	}
	
	// update bounding boxes of child elements
	int bbx = m_bounds.x + padding.left;
	int bby = m_bounds.y + padding.top;
	int contentWidth = m_bounds.width - padding.left - padding.right;

	// pass 2 : assign remaining vertical space to children 
	for (auto child : m_children) {
		Size fixed = child->getFixedSize();
		BoundingBox childBB;
		childBB.x = bbx;
		childBB.y = bby;

		if (fixed.hasWidth()) {
			childBB.width = fixed.width;
		} else {
			Size desired = child->getDesiredSize(engine);
			SizePolicy horizontalPolicy = child->getHorizontalSizePolicy();
			if (desired.hasWidth() && horizontalPolicy == SizePolicy::Preferred) {
				childBB.width = desired.width;
			} else {
				childBB.width = contentWidth;
			}
		}
	
		if (fixed.hasHeight()) {
			childBB.height = fixed.height;
		} else {
			childBB.height = 10;	// XXX
		}
	}
}

}
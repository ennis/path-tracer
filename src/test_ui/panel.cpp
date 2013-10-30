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

void Panel::render(sf::RenderTarget &renderTarget)
{
	//std::clog << "Panel::render\n";
	getEngine().drawPanelFrame(renderTarget, *this);
	for (auto child : m_children) {
		child->render(renderTarget);
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

Size Panel::getDesiredSize()
{
	return measureFromContents(Size(-1, -1));
}

void Panel::doLayout()
{
	Engine &engine = getEngine();
	Margins margin, padding;
	engine.getPanelMargins(margin, padding);

	int totalHeight = m_bounds.height;
	int remHeight = totalHeight;

	// 0 : remove padding and spacing
	remHeight -= padding.top + padding.bottom;
	// 2px spacing (XXX engine.getPanelItemSpacing)
	remHeight -= m_children.size() < 1 ? 0 : m_children.size() * 2;
	
	// update bounding boxes of child elements
	int bbx = m_bounds.x + padding.left;
	int bby = m_bounds.y + padding.top;
	int contentWidth = m_bounds.width - padding.left - padding.right;
	int numFillChildren = 0;
	
	// pass 1 : assign space to non-filling children
	for (auto child : m_children) {
		Size desired = child->getDesiredSize();
		if (desired.height == Size::FILL) {
			numFillChildren++;
		} else {
			remHeight -= desired.height;
		}
	}
	
	int fillingChildHeight;
	if (numFillChildren != 0) {
		fillingChildHeight = remHeight / numFillChildren;
	} else {
		fillingChildHeight = 0;	// XXX unused anyway
	}
	
	// pass 2 : assign remaining vertical space to children 
	for (auto child : m_children) {
		Size desired = child->getDesiredSize();

		BoundingBox childBB;
		childBB.x = bbx;
		childBB.y = bby;
		
		if (desired.width == Size::FILL) {
			childBB.width = contentWidth;
		} else {
			childBB.width = std::min(contentWidth, desired.width);
		}
		
		if (desired.height == Size::FILL) {
			childBB.height = fillingChildHeight;
		} else {
			childBB.height = desired.height;
		}

		child->layout(childBB);
		bby += childBB.height + 2;
	}
}

}
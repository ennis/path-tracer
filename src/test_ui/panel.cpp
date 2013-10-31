#include "panel.hpp"
#include "engine.hpp"

#include <iostream>

namespace ui
{

Panel::Panel(Panel::private_key) : m_orientation(VERTICAL)
{
	std::clog << "Panel\n";
}

Panel::Panel(private_key, Orientation orientation) : m_orientation(orientation)
{
	std::clog << "Panel(Orientation)\n";
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
	int maxWidth = -1;
	int maxHeight = -1;
	for (auto child : m_children) {
		Size desired = child->getDesiredSize();
		if (desired.width > maxWidth) maxWidth = desired.width;
		if (desired.height > maxHeight) maxHeight = desired.height;
	}
	return measureFromContents(Size(maxWidth, maxHeight));
}

void Panel::doLayout()
{
	Engine &engine = getEngine();
	Margins margin, padding;
	engine.getPanelMargins(margin, padding);

	int totalHeight = m_bounds.height;
	int remSize;

	// 0 : remove padding and spacing
	if (m_orientation == VERTICAL) {
		remSize = m_bounds.height;
		remSize -= padding.top + padding.bottom;	
	} else {
		remSize = m_bounds.width;
		remSize -= padding.left + padding.right;	
	}

	// 2px spacing (XXX engine.getPanelItemSpacing)
	remSize -= m_children.size() < 1 ? 0 : m_children.size() * 2;
	
	// update bounding boxes of child elements
	int bbx = m_bounds.x + padding.left;
	int bby = m_bounds.y + padding.top;
	int contentSize;
	if (m_orientation == VERTICAL) {
		contentSize = m_bounds.width - padding.left - padding.right;
	} else {
		contentSize = m_bounds.height - padding.top - padding.bottom;
	}
	int numFillChildren = 0;
	
	// pass 1 : assign space to non-filling children
	for (auto child : m_children) {
		Size desired = child->getDesiredSize();
		if (m_orientation == VERTICAL) {
			if (desired.height == Size::FILL) {
				numFillChildren++;
			} else {
				remSize -= desired.height;
			}
		} else {
			if (desired.width == Size::FILL) {
				numFillChildren++;
			} else {
				remSize -= desired.width;
			}
		}
	}
	
	int fillingChildSize;
	
	if (numFillChildren != 0) {
		fillingChildSize = remSize / numFillChildren;
	} else {
		fillingChildSize = 0;	// unused anyway
	}
	
	// pass 2 : assign remaining vertical space to children 
	for (auto child : m_children) {
		Size desired = child->getDesiredSize();

		BoundingBox childBB;
		childBB.x = bbx;
		childBB.y = bby;

		// vertical
		if (m_orientation == VERTICAL) {
			if (desired.width == Size::FILL) {
				childBB.width = contentSize;
			} else {
				childBB.width = std::min(contentSize, desired.width);
			}
		
			if (desired.height == Size::FILL) {
				childBB.height = fillingChildSize;
			} else {
				childBB.height = desired.height;
			}

			bby += childBB.height + 2;

		} else {
			if (desired.width == Size::FILL) {
				childBB.width = fillingChildSize;
			} else {
				childBB.width = desired.width;
			}
		
			if (desired.height == Size::FILL) {
				childBB.height = contentSize;
			} else {
				childBB.height = std::min(contentSize, desired.height);
			}

			bbx += childBB.width + 2;
		}

		child->layout(childBB);
	}
}

}
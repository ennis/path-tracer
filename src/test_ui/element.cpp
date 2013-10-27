#include "element.hpp"

namespace ui
{

/*void Element::calculateRequisition()
{
	if (this->m_boxWidth.type != ui::Size::PIXELS) {
		m_requisition.width = -1;
	} else {
		m_requisition.width = this->m_boxWidth.value + m_margin.left + m_margin.right;
	}

	if (this->m_boxHeight.type != ui::Size::PIXELS) {
		m_requisition.height = -1;
	} else {
		m_requisition.height = this->m_boxHeight.value + m_margin.bottom + m_margin.top;
	}
}*/

// sets m_bounds and m_contents
/*void Element::layoutBox(Margins const &styleMargin, Margins const &stylePadding)
{
	Margins margin = m_margin;
	margin.add(styleMargin);
	Margins padding = m_padding;
	padding.add(stylePadding);

	if (m_requisition.width == -1) {
		m_bounds.width = m_boxWidth.toPixels(m_allocation.width) - margin.left - margin.right;
		m_bounds.x = m_allocation.x + margin.left;
	} else {
		switch (m_placement) {
		case TL:
		case CL:
		case BL:
			m_bounds.x = m_allocation.x + margin.left;
			break;
		case TC:
		case CC:
		case BC:
			m_bounds.x = m_allocation.x + std::max(0, (m_allocation.width - m_requisition.width) / 2);
			break;
		case TR:
		case CR:
		case BR:
			m_bounds.x = m_allocation.x + m_allocation.width - m_requisition.width - margin.right;
			break;
		}
		m_bounds.width = m_requisition.width - margin.right - margin.left;
	}
	if (m_requisition.height == -1) {
		m_bounds.height = m_boxHeight.toPixels(m_allocation.height) - margin.top - margin.bottom;
		m_bounds.y = m_allocation.y + margin.top;
	} else {
		switch (m_placement) {
		case TL:
		case TC:
		case TR:
			m_bounds.y = m_allocation.y + margin.top;
			break;
		case CL:
		case CC:
		case CR:
			m_bounds.y = m_allocation.y + std::max(0, (m_allocation.height - m_requisition.height) / 2);
			break;
		case BL:
		case BC:
		case BR:
			m_bounds.y = m_allocation.y + m_allocation.height - m_requisition.height - margin.bottom;
			break;
		}
		m_bounds.height = m_requisition.height - margin.top - margin.bottom;
	}

	m_contents = m_bounds.applyMargins(padding);
}*/

/*void Element::setAllocation(BoundingBox const &newBB)
{
	std::clog << "Element::setAllocation " << newBB << "\n";
	m_allocation = newBB;
	// box placement
	doPlacement();
	onSetAllocation();
}

void Element::onSetAllocation()
{
}*/


void Element::onHoverEnter() {
	onHoverEnterEvent(shared_from_this());
}

void Element::onHoverLeave() {
	onHoverLeaveEvent(shared_from_this());
}

void Element::onClick(int mouseX, int mouseY) {
	onClickEvent(shared_from_this(), mouseX, mouseY);
}

void Element::onKeyDown(sf::Event::KeyEvent const &keyEvent) {
	onKeyDownEvent(shared_from_this(), keyEvent);
}

void Element::onKeyUp(sf::Event::KeyEvent const &keyEvent) {
	onKeyUpEvent(shared_from_this(), keyEvent);
}

bool Element::hitTest(int x, int y) const
{
	if ((m_bounds.x < x) && 
		(x < (m_bounds.x + m_bounds.width)) && 
		(m_bounds.y < y) && 
		(y < (m_bounds.y + m_bounds.height))) { 
		return true;
	} else {
		return false;
	}
}

bool Element::handleEvent(sf::Event const &event)
{
	bool handled = false;
	switch (event.type) {
	case sf::Event::MouseMoved:
		{
			bool hit = hitTest(event.mouseMove.x, event.mouseMove.y);
			if (hit != testState(HOVER)) {
				//std::cout << "handleMouseMove (" << this << ") mouse " << (hit ? "entering" : "leaving") << '\n';
				if (hit == true) {
					setState(HOVER);
					onHoverEnter();
				} else {
					clearState(HOVER);
					onHoverLeave();
				}
			}
		}
		handled = true;
		break;

	case sf::Event::MouseButtonPressed:
		if (hitTest(event.mouseButton.x, event.mouseButton.y)) {
			onClick(event.mouseButton.x, event.mouseButton.y);
			handled = true;
		} else {
			handled = false;
		}
		break;

	default:
		handled = false;
		break;
	}
	return handled;
}

/*void Element::setWidth(Size const &width) 
{
	m_boxWidth = width;
}

void Element::setHeight(Size const &height)
{
	m_boxHeight = height;
}*/

//============================
// State query and manipulation

bool Element::testState(int stateBit) const {
	return (m_state & stateBit) == stateBit;
}

void Element::setState(int stateBit) {
	m_state |= stateBit;
}

void Element::clearState(int stateBit) {
	m_state &= ~stateBit;
}

}
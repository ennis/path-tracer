#include "button.hpp"

namespace ui
{

Button::Button(Button::private_key, std::string const &label) : m_label(TextBox::create(label))
{

}

void Button::render(sf::RenderTarget &renderTarget, Engine &engine)
{
	engine.drawButtonFrame(renderTarget, *this);
	engine.drawTextBox(renderTarget, *m_label.get());
}

void Button::onSetAllocation()
{
	int bbx = m_bounds.x + m_paddingLeft;
	int bby = m_bounds.y + m_paddingTop;
	BoundingBox childBB(bbx, bby, m_bounds.width - m_paddingLeft - m_paddingRight, m_bounds.height - m_paddingTop - m_paddingBottom);
	m_label->setAllocation(childBB);
}

}
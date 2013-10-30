#include "button.hpp"

namespace ui
{

Button::Button(Button::private_key, std::string const &label) : m_label(TextBox::create(label))
{
	setSize(Size(Size::ADJUST, Size::ADJUST));
}

void Button::doLayout()
{
	Margins margin, padding;
	getEngine().getButtonMargins(margin, padding);
	m_label->layout(m_bounds.contract(padding));
}

Size Button::getDesiredSize()
{
	Margins margin, padding;
	getEngine().getButtonMargins(margin, padding);
	Size contents = m_label->getDesiredSize().expand(padding);
	return measureFromContents(contents);	
}

void Button::render(sf::RenderTarget &renderTarget)
{
	Engine &engine = getEngine();
	engine.drawButtonFrame(renderTarget, *this);
	engine.drawTextBox(renderTarget, *m_label.get());
}


}
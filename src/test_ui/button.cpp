#include "button.hpp"

namespace ui
{

Button::Button(Button::private_key, std::string const &label) : m_label(TextBox::create(label))
{
	setSize(Size(Size::ADJUST, Size::ADJUST));
}

void Button::doLayout(Engine &engine)
{
	Margins margin, padding;
	engine.getButtonMargins(margin, padding);
	m_label->layout(engine, m_bounds.contract(padding));
}

Size Button::getDesiredSize(Engine &engine)
{
	Margins margin, padding;
	engine.getButtonMargins(margin, padding);
	Size contents = m_label->getDesiredSize(engine).expand(padding);
	return measureFromContents(contents);	
}

void Button::render(sf::RenderTarget &renderTarget, Engine &engine)
{
	engine.drawButtonFrame(renderTarget, *this);
	engine.drawTextBox(renderTarget, *m_label.get());
}


}
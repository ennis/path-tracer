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


}
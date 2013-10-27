#include "textbox.hpp"

namespace ui
{

TextBox::~TextBox()
{
}

Size TextBox::getDesiredSize(Engine &engine)
{
	return engine.measureText(m_string);
}

void TextBox::render(sf::RenderTarget &renderTarget, Engine &engine)
{
	engine.drawTextBox(renderTarget, *this);
}

}
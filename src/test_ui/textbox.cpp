#include "textbox.hpp"

namespace ui
{

TextBox::~TextBox()
{
}

Size TextBox::getDesiredSize()
{
	return getEngine().measureText(m_string);
}

void TextBox::render(sf::RenderTarget &renderTarget)
{
	getEngine().drawTextBox(renderTarget, *this);
}

}
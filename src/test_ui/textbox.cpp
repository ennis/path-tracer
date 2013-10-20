#include "textbox.hpp"

namespace ui
{

TextBox::~TextBox()
{
}

void TextBox::render(sf::RenderTarget &renderTarget, Engine &engine)
{
	engine.drawTextBox(renderTarget, *this);
}

}
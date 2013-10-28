#include "checkbox.hpp"

namespace ui 
{

void CheckBox::doLayout(ui::Engine& engine)
{
	// XXX nothing to do
}

Size CheckBox::getDesiredSize(Engine& engine)
{
	return engine.getCheckboxSize();
}

void CheckBox::render(sf::RenderTarget& renderTarget, Engine& engine)
{
	engine.drawCheckBox(renderTarget, *this);
}

void CheckBox::onClick(int mouseX, int mouseY)
{
	m_checked = !m_checked;
	Element::onClick(mouseX, mouseY);
}



}

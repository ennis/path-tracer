#include "checkbox.hpp"

namespace ui 
{

void CheckBox::doLayout()
{
	// XXX nothing to do
}

Size CheckBox::getDesiredSize()
{
	return getEngine().getCheckboxSize();
}

void CheckBox::render(sf::RenderTarget& renderTarget)
{
	getEngine().drawCheckBox(renderTarget, *this);
}

void CheckBox::onClick(int mouseX, int mouseY)
{
	m_checked = !m_checked;
	Element::onClick(mouseX, mouseY);
}



}

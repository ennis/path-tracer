#include "slider.hpp"

namespace ui
{
	
void Slider::doLayout()
{
	// XXX nothing to do
}

void Slider::onClick(int mouseX, int mouseY)
{
	int newValue = getEngine().getSliderPosition(*this, mouseX, mouseY);
	setValue(newValue);
	ui::Element::onClick(mouseX, mouseY);
}

void Slider::render(sf::RenderTarget &renderTarget)
{
	getEngine().drawSlider(renderTarget, *this);
}

Size Slider::getDesiredSize()
{
	return Size(Size::FILL, getEngine().getSliderHeight());
}

	
}
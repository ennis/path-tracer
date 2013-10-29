#include "slider.hpp"

namespace ui
{
	
void Slider::doLayout(Engine& engine)
{
	
}

void Slider::onDrag(int mouseX, int mouseY)
{
	ui::Element::onDrag(mouseX, mouseY);
}


Size Slider::getDesiredSize(Engine& engine)
{
	return ui::Element::getDesiredSize(engine);
}

	
}
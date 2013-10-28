#include "slider.hpp"

namespace ui
{
	
void Slider::doLayout(Engine& engine)
{
	
}

void Slider::onDrag(int mouseX, int mouseY)
{
	m_sliderPosition = engine.mouseCoordsToSliderValue(mouseX - bounds.X, mouseY - bounds.Y)
	ui::Element::onDrag(mouseX, mouseY);
}


	
}
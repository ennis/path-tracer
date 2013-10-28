#ifndef SLIDER_HPP
#define SLIDER_HPP

#include "element.hpp"

namespace ui
{

class Slider : public Element
{
	struct private_key{};
public:
	Slider(private_key);
	virtual ~Slider();
	
	virtual Size getDesiredSize(Engine& engine);
	virtual void doLayout(Engine& engine);
	virtual void onDrag(int mouseX, int mouseY);
	
private:
	int m_sliderPosition;
	int m_sliderWidth;
};
	
}

#endif
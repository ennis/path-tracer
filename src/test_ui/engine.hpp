#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "boundingbox.hpp"

#include <SFML/Graphics.hpp>

#include <memory>
#include <cstdint>

namespace ui
{

class Element;
class Button;
class Panel;
class TextBox;

class Engine
{
public:
	typedef std::shared_ptr<Engine> Ptr;

	virtual void drawButtonFrame(sf::RenderTarget &renderTarget, Button &button) = 0;
	virtual void drawPanelFrame(sf::RenderTarget &renderTarget, Panel &panel) = 0;
	//virtual void drawSlider(sf::RenderTarget &renderTarget, BoundingBox const &bounds, int state, int sliderPosition) = 0;
	virtual void drawTextBox(sf::RenderTarget &renderTarget, TextBox &textBox) = 0;

	// TODO getPanelMargins, getButtonMargins, etc.

	virtual void getCheckboxSize(int &width, int &height) = 0;
	virtual int getSliderHeight() = 0;
	virtual void getMinButtonSize(int &width, int &height) = 0;

protected:
};

}

#endif 

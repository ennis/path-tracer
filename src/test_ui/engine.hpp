#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "boundingbox.hpp"
#include "size.hpp"

#include <SFML/Graphics.hpp>

#include <memory>
#include <cstdint>

namespace ui
{

class Element;
class Button;
class Panel;
class TextBox;

enum ElementType
{
	ET_Panel,
	ET_Button,
	ET_TextBox
};

class Engine
{
public:
	typedef std::shared_ptr<Engine> Ptr;

	virtual void drawButtonFrame(sf::RenderTarget &renderTarget, Button &button) = 0;
	virtual void drawPanelFrame(sf::RenderTarget &renderTarget, Panel &panel) = 0;
	//virtual void drawSlider(sf::RenderTarget &renderTarget, BoundingBox const &bounds, int state, int sliderPosition) = 0;
	virtual void drawTextBox(sf::RenderTarget &renderTarget, TextBox &textBox) = 0;

	// TODO getPanelMargins, getButtonMargins, etc.

	virtual void getButtonMargins(Margins &margin, Margins &padding) = 0;
	virtual void getPanelMargins(Margins &margin, Margins &padding) = 0;
	virtual void getTextBoxMargins(Margins &margin, Margins &padding) = 0;

	virtual void getCheckboxSize(int &width, int &height) = 0;
	virtual int getSliderHeight() = 0;
	virtual void getMinButtonSize(int &width, int &height) = 0;

	virtual Size measureText(std::string const &str) = 0;

protected:
};

}

#endif 

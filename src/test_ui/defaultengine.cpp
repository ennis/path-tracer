#include "defaultengine.hpp"

#include "button.hpp"
#include "textbox.hpp"
#include "panel.hpp"
#include "checkbox.hpp"
#include "radiobutton.hpp"

namespace ui
{

const sf::Color DefaultEngine::buttonBorderColor = sf::Color::Red;
const sf::Color DefaultEngine::buttonBorderHoverColor = sf::Color::Red;
const sf::Color DefaultEngine::buttonBackgroundColor = sf::Color::Blue;
const sf::Color DefaultEngine::buttonBackgroundHoverColor = sf::Color::Blue;
const sf::Color DefaultEngine::buttonForegroundColor = sf::Color::Black;
const sf::Color DefaultEngine::buttonForegroundHoverColor = sf::Color::Black;
const sf::Color DefaultEngine::panelBorderColor = sf::Color();
const sf::Color DefaultEngine::panelBorderHoverColor = sf::Color(54, 31, 171, 160);
const sf::Color DefaultEngine::panelBackgroundColor = sf::Color(54, 31, 171, 160);
const sf::Color DefaultEngine::panelBackgroundHoverColor =  sf::Color(54, 31, 171, 210);

const sf::Color DefaultEngine::checkBoxBorderColor = sf::Color(112, 244, 198);
const sf::Color DefaultEngine::checkBoxBackgroundColor = sf::Color(112, 167, 244);
const sf::Color DefaultEngine::checkBoxBackgroundHoverColor = sf::Color(171, 201, 244);
const sf::Color DefaultEngine::checkBoxTickColor = sf::Color(112, 244, 198);
const Margins DefaultEngine::checkBoxTickPadding = Margins(3, 3, 3, 3);
const Size DefaultEngine::checkBoxSize = Size(12, 12);
const Size DefaultEngine::radioButtonSize = Size(12, 12);

DefaultEngine::~DefaultEngine()
{
}

void DefaultEngine::drawRadioButton(sf::RenderTarget& renderTarget, RadioButton& radioButton)
{
	BoundingBox const &bb = radioButton.getBounds();
	bool hover = radioButton.testState(Element::HOVER);
	sf::CircleShape circle;
	circle.setPosition(static_cast<float>(bb.x)+1, static_cast<float>(bb.y)+1);
	circle.setRadius(5);
	circle.setFillColor(hover ? checkBoxBackgroundHoverColor : checkBoxBackgroundColor);
	circle.setOutlineColor(checkBoxBorderColor);
	circle.setOutlineThickness(1);
	renderTarget.draw(circle);
	
	if (radioButton.isChecked()) {
		circle.setPosition(static_cast<float>(bb.x)+2, static_cast<float>(bb.y)+2);
		circle.setRadius(4);
		circle.setFillColor(checkBoxTickColor);
		circle.setOutlineThickness(0);
		renderTarget.draw(circle);
	}
}


void DefaultEngine::drawButtonFrame(sf::RenderTarget &renderTarget, Button &button)
{
	// TODO draw a (rounded) rectangle and borders
	BoundingBox const &bb = button.getBounds();
	bool hover = button.testState(Element::HOVER);
	drawFilledRectangle(renderTarget, bb.x, bb.y, bb.width, bb.height, hover ? buttonBackgroundHoverColor : buttonBackgroundColor);
	drawBorders(renderTarget, bb, 2, 2, 2, 2, hover ? buttonBorderHoverColor : buttonBorderColor);
}

void DefaultEngine::drawPanelFrame(sf::RenderTarget &renderTarget, Panel &panel)
{
	BoundingBox const &bb = panel.getBounds();
	bool hover = panel.testState(Element::HOVER);
	drawFilledRectangle(renderTarget, bb.x, bb.y, bb.width, bb.height, hover ? panelBackgroundHoverColor : panelBackgroundColor);
}

/*void DefaultEngine::drawSlider(sf::RenderTarget &renderTarget, BoundingBox const &bounds, int state, int sliderPosition)
{
	// TODO draw a line and a circle/point
}*/

void DefaultEngine::drawTextBox(sf::RenderTarget &renderTarget, TextBox &textBox)
{
	BoundingBox const &bb = textBox.getBounds();
	drawText(renderTarget, bb.x, bb.y, 12, textBox.getText(), buttonForegroundColor);
}

Size DefaultEngine::getCheckboxSize()
{
	return checkBoxSize;
}

Size DefaultEngine::getRadioSize()
{
	return radioButtonSize;
}

int DefaultEngine::getSliderHeight()
{
	// TODO Dummy
	return 8;
}

void DefaultEngine::getMinButtonSize(int &width, int &height)
{
	// TODO Dummy
	width = 8;
	height = 8;
}

void DefaultEngine::drawFilledRectangle(sf::RenderTarget &renderTarget, int x, int y, int width, int height, sf::Color const &color)
{
	sf::RectangleShape rect;
	rect.setSize(sf::Vector2f(static_cast<float>(width), static_cast<float>(height)));
	rect.setPosition(sf::Vector2f(static_cast<float>(x), static_cast<float>(y)));
	rect.setOutlineThickness(0);
	rect.setFillColor(color);
	renderTarget.draw(rect);
}

void DefaultEngine::drawBorders(sf::RenderTarget &renderTarget, BoundingBox const &bb, int left, int right, int top, int bottom, sf::Color const &color)
{
	drawFilledRectangle(renderTarget, bb.x, bb.y, bb.width, top, color);
	drawFilledRectangle(renderTarget, bb.x, bb.y, left, bb.height, color);
	drawFilledRectangle(renderTarget, bb.x, bb.y + bb.height - bottom, bb.width, bottom, color);
	drawFilledRectangle(renderTarget, bb.x + bb.width - right, bb.y, right, bb.height, color);
}

void DefaultEngine::drawText(sf::RenderTarget &renderTarget, int x, int y, int fontSize, std::string const &text, sf::Color const &color)
{
	sf::Text textBox(text, m_defaultFont);
	textBox.setPosition(static_cast<float>(x), static_cast<float>(y));
	textBox.setCharacterSize(18);
	textBox.setColor(color);
	renderTarget.draw(textBox);
}

void DefaultEngine::getButtonMargins(Margins &margin, Margins &padding) 
{
	margin = Margins(1,1,1,1);
	padding = Margins(1,1,1,1);
}

void DefaultEngine::getPanelMargins(Margins &margin, Margins &padding)
{
	margin = Margins(0,0,0,0);
	padding = Margins(1,1,1,1);
}

void DefaultEngine::getTextBoxMargins(Margins &margin, Margins &padding)
{
	margin = Margins(0,0,0,0);
	padding = Margins(0,0,0,0);
}

Size DefaultEngine::measureText(std::string const &str)
{
	sf::Text textBox(str, m_defaultFont);
	return Size(textBox.getGlobalBounds().width, textBox.getGlobalBounds().height);
}

void DefaultEngine::drawCheckBox(sf::RenderTarget &renderTarget, CheckBox& checkBox)
{
	bool hover = checkBox.testState(Element::HOVER);
	//bool checked = checkBox.isChecked();
	BoundingBox bounds = checkBox.getBounds();
	drawFilledRectangle(renderTarget, 
		bounds.x, 
		bounds.y, 
		bounds.width, 
		bounds.height, 
		hover ? checkBoxBackgroundHoverColor : checkBoxBackgroundColor);
	
	drawBorders(renderTarget, bounds, 1, 1, 1, 1, checkBoxBorderColor);
	
	if (checkBox.isChecked()) {
		BoundingBox check = bounds.contract(checkBoxTickPadding);
		drawFilledRectangle(renderTarget, check.x, check.y, check.width, check.height, checkBoxTickColor);
	}
	
}

}
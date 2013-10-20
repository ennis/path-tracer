#include "defaultengine.hpp"

#include "button.hpp"
#include "textbox.hpp"
#include "panel.hpp"

namespace ui
{

const sf::Color DefaultEngine::buttonBorderColor = sf::Color::Red;
const sf::Color DefaultEngine::buttonBorderHoverColor = sf::Color::Red;
const sf::Color DefaultEngine::buttonBackgroundColor = sf::Color::Blue;
const sf::Color DefaultEngine::buttonBackgroundHoverColor = sf::Color::Blue;
const sf::Color DefaultEngine::buttonForegroundColor = sf::Color::Black;
const sf::Color DefaultEngine::buttonForegroundHoverColor = sf::Color::Black;
const sf::Color DefaultEngine::panelBorderColor = sf::Color();
const sf::Color DefaultEngine::panelBorderHoverColor = sf::Color();
const sf::Color DefaultEngine::panelBackgroundColor = sf::Color(54, 31, 171, 160);
const sf::Color DefaultEngine::panelBackgroundHoverColor = sf::Color();

DefaultEngine::~DefaultEngine()
{
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
	// TODO draw a semitransparent rectangle
	BoundingBox const &bb = panel.getBounds();
	drawFilledRectangle(renderTarget, bb.x, bb.y, bb.width, bb.height, panelBackgroundColor);
}

/*void DefaultEngine::drawSlider(sf::RenderTarget &renderTarget, BoundingBox const &bounds, int state, int sliderPosition)
{
	// TODO draw a line and a circle/point
}*/

void DefaultEngine::drawTextBox(sf::RenderTarget &renderTarget, TextBox &textBox)
{
	// TODO draw text with default font
	BoundingBox const &bb = textBox.getBounds();
	drawText(renderTarget, bb.x, bb.y, 12, textBox.getText(), buttonForegroundColor);
}

/*void DefaultEngine::drawCheckbox(sf::RenderTarget &renderTarget, BoundingBox const &bounds, int state, bool checked)
{
	// TODO draw borders and a check mark
}

void DefaultEngine::drawButtonLabel(sf::RenderTarget &renderTarget, BoundingBox const &bounds, int state, std::string const &text)
{
	// drawText
}*/

void DefaultEngine::getCheckboxSize(int &width, int &height)
{
	// TODO Dummy
	width = 8;
	height = 8;
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

}
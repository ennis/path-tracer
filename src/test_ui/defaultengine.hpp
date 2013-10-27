#ifndef DEFAULTENGINE_HPP
#define DEFAULTENGINE_HPP

#include <memory>
#include <SFML/Graphics.hpp>

#include "engine.hpp"

namespace ui
{

class DefaultEngine : public Engine
{
	struct private_key {};
public:
	typedef std::shared_ptr<DefaultEngine> Ptr;
	
	~DefaultEngine();

	virtual void drawButtonFrame(sf::RenderTarget &renderTarget, Button &button);
	virtual void drawPanelFrame(sf::RenderTarget &renderTarget, Panel &panel);
	//virtual void drawSlider(sf::RenderTarget &renderTarget, BoundingBox const &bounds, int state, int sliderPosition);
	virtual void drawTextBox(sf::RenderTarget &renderTarget, TextBox &textBox);
	//virtual void drawCheckbox(sf::RenderTarget &renderTarget, BoundingBox const &bounds, int state, bool checked);
	//virtual void drawButtonLabel(sf::RenderTarget &renderTarget, BoundingBox const &bounds, int state, std::string const &text);
	
	virtual void getButtonMargins(Margins &margin, Margins &padding);
	virtual void getPanelMargins(Margins &margin, Margins &padding);
	virtual void getTextBoxMargins(Margins &margin, Margins &padding);

	virtual void getCheckboxSize(int &width, int &height);
	virtual int getSliderHeight();
	virtual void getMinButtonSize(int &width, int &height);

	DefaultEngine(private_key, sf::Font const &defaultFont) : m_defaultFont(defaultFont)
	{}

	static Ptr create(sf::Font const &defaultFont)
	{
		return std::make_shared<DefaultEngine>(private_key(), defaultFont);
	}

	static const sf::Color buttonBorderColor;
	static const sf::Color buttonBorderHoverColor;
	static const sf::Color buttonBackgroundColor;
	static const sf::Color buttonBackgroundHoverColor;
	static const sf::Color buttonForegroundColor;
	static const sf::Color buttonForegroundHoverColor;
	static const sf::Color panelBorderColor;
	static const sf::Color panelBorderHoverColor;
	static const sf::Color panelBackgroundColor;
	static const sf::Color panelBackgroundHoverColor;

protected:

	void drawFilledRectangle(sf::RenderTarget &renderTarget, int x, int y, int width, int height, sf::Color const &color);
	void drawBorders(sf::RenderTarget &renderTarget, BoundingBox const &bb, int left, int right, int top, int bottom, sf::Color const &color);
	void drawText(sf::RenderTarget &renderTarget, int x, int y, int fontSize, std::string const &text, sf::Color const &color);

	sf::Font const &m_defaultFont;
};

}

#endif

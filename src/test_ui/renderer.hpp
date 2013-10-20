#include <SFML/Graphics.hpp>
#include <vector>

class Primitive
{
public:
	virtual void render(sf::RenderTarget &target);
private:
};

class RectanglePrimitive : public Primitive
{
public:
	RectanglePrimitive(int x, int y, int width, int height, sf::Color const &color) :
		m_x(x), m_y(y), m_width(width), m_height(height)
	{}

	virtual void render(sf::RenderTarget &target) {
		sf::RectangleShape rectangleShape;
		rectangleShape.setSize(sf::Vector2f(static_cast<float>(m_width), static_cast<float>(m_height)));
		rectangleShape.setPosition(sf::Vector2f(static_cast<float>(m_x), static_cast<float>(m_y)));
		rectangleShape.setOutlineThickness(0);
		rectangleShape.setFillColor(m_color);
		target.draw(rectangleShape);
	}

private:
	int m_x, m_y, m_width, m_height;
	sf::Color m_color;
};

class TextPrimitive : public Primitive
{
public:
	TextPrimitive(char const *text, sf::Font const &font, int fontHeight) 
	{
		m_text.setString(text);
		m_text.setFont(font);
		m_text.setCharacterSize(fontHeight);
	}

	virtual void render(sf::RenderTarget &target) {
		target.draw(m_text);
	}

private:
	sf::Text m_text;
};


class RenderList
{
public:
	RenderList()
	{}

	void replay(sf::RenderTarget &target) {
		for (Primitive *p : m_primitives) {
			p->render(target);
		}
	}

	void clear() {
		for (Primitive *p : m_primitives) {
			delete p;
		}
		m_primitives.clear();
	}

	void drawRectangle(int x, int y, int width, int height, sf::Color const &color) {
		m_primitives.push_back(new RectanglePrimitive(x, y, width, height, color));
	}

	void drawText(char const *text, sf::Font const &font, int fontHeight) {
		m_primitives.push_back(new TextPrimitive(text, font, fontHeight));
	}

private:
	std::vector<Primitive*> m_primitives;
};
#include <GL/glew.h>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <sstream>
#include <iostream>

#define glCheck(call) { call; glCheckError(#call); }

static const unsigned int WIDTH = 800;
static const unsigned int HEIGHT = 600;

void glCheckError(char const * call)
{
	int err = glGetError();
	if (err != GL_NO_ERROR) {
		std::stringstream ss;
		ss << call << " -> " << err << '\n';
		OutputDebugStringA(ss.str().c_str());
	}
}

/*enum WindowEventType
{
	MOUSE_CLICKED,
	MOUSE_RELEASED
};

static const char *getWindowEventTypeString(WindowEventType type)
{
	switch (type)
	{
	case MOUSE_CLICKED:
		return "MOUSE_CLICKED";
	case MOUSE_RELEASED:
		return "MOUSE_RELEASED";
	}
	return "<invalid>";
}

struct WindowEvent
{
	Window *window;
	WindowEventType type;
};*/

/*class Window
{
public:
	Window(int width, int height)
	{}

	~Window()
	{}

	void addEventHandler(WindowEventHandler const *eventHandler)
	{
		m_eventHandlers.push_back(eventHandler);
	}

	void sendEvent(WindowEvent const *windowEvent)
	{
		for (auto i = m_eventHandlers.begin(); 
			i != m_eventHandlers.end(); ++i)
		{
			(*i)->onWindowEvent(windowEvent);
		}
	}

private:
	std::vector<WindowEventHandler const *> m_eventHandlers;
};*/


/*enum UIEventType
{
	MOUSE_MOVED,	// hittest
	MOUSE_CLICKED,
	MOUSE_RELEASED,
	HOVER_IN,
	HOVER_OUT,
	CLICKED,
	FOCUS
};

struct UIEvent
{
	UIElement *uiElement;
	UIEventType type;
	int relX, relY;
};*/

struct UISize
{
	enum UISizeType {
		PERCENTAGE,
		SIZE_PIXELS,
		SIZE_ADJUST
	};

	UISizeType type;
	int value;		// -1 is auto or 100%

	UISize() : value(0), type(SIZE_PIXELS)
	{}

	UISize(int v, UISizeType t = SIZE_PIXELS) : value(v), type(t)
	{}

	static UISize percentage(int percent) {
		UISize ret;
		ret.type = PERCENTAGE;
		ret.value = percent;
		return ret;
	}

	static UISize px(int pixels) {
		UISize ret;
		ret.type = SIZE_PIXELS;
		ret.value = pixels;
		return ret;
	}

	int toPixels(int containingBlockSize) {
		if (type == PERCENTAGE) {
			return containingBlockSize * value / 100;
		} else if (type == SIZE_PIXELS) {
			return value;
		} else {
			return value;
		}
	}

	void dump(std::ostream &os) const {
		if (value == -1) {
			os << "auto";
		} else {
			os << value << (type == SIZE_PIXELS) ? "px" : "%";
		}
	}

	bool operator==(UISize const &rhs) const {
		return value == rhs.value && type == rhs.type;
	}

	static UISize AUTO;
	static UISize ADJUST;
};

UISize UISize::AUTO = UISize(-1);
UISize UISize::ADJUST = UISize(-1, UISize::SIZE_ADJUST);

enum UIAlign
{
	ALIGN_RIGHT,
	ALIGN_LEFT,
	ALIGN_CENTER,
	ALIGN_FLOAT
};

// bounding box
struct UIBoundingBox
{
	UIBoundingBox() : bbX(0), bbY(0), bbWidth(0), bbHeight(0)
	{}

	UIBoundingBox(int x, int y, int width, int height) : bbX(x), bbY(y), bbWidth(width), bbHeight(height)
	{}

	int bbX;
	int bbY;
	int bbWidth;
	int bbHeight;

	void dump(std::ostream &os) const {
		os << bbX << ',' << bbY << ':' << bbWidth << 'x' << bbHeight;
	}
};

// child->updateMetrics(containingBoxWidth, containingBoxHeight)
// child->getCalculatedMetrics(...) -> compute bounding box
// child->setAllocation(UILayoutBox)
//

class UIElement;

//=======================================================
class UI
{
public:

	static const sf::Color DEFAULT_BACKGROUND_COLOR;
	static const sf::Color DEFAULT_BORDER_COLOR;
	static const sf::Color DEFAULT_BACKGROUND_HOVER_COLOR;

	UI(int width, int height) : m_width(width), m_height(height), m_root(NULL), m_doLayout(true)
	{
		std::cout << "UI (" << width << 'x' << height << ")\n";
		m_defaultFont.loadFromFile("uzura.ttf");
	}

	void setRoot(UIElement *elem) 
	{
		m_root = elem;
		m_doLayout = true;
	}

	void setSize(int width, int height)
	{
		m_width = width;
		m_height = height;
		m_doLayout = true;
	}

	void onEvent(sf::Event const &event);

	void updateLayout();

	void render(sf::RenderTarget &renderTarget);

	sf::Font const &getDefaultFont() const {
		return m_defaultFont;
	}

private:
	int m_width;
	int m_height;
	UIElement *m_root;
	sf::Font m_defaultFont;
	bool m_doLayout;
};

static inline int alignPosition(int objSize, int containerSize, UIAlign align)
{
	switch (align) {
	case ALIGN_RIGHT:
		return std::max(0, containerSize - objSize);
	case ALIGN_CENTER:
		return std::max(0, (containerSize - objSize) / 2);
	case ALIGN_LEFT:
		return 0;
	default:
		return 0;
	}
}

//====================================
// UIElement
class UIElement
{
public:
	UIElement() : 
		m_boxWidth(-1),
		m_boxHeight(-1),
		m_marginBoxWidth(0),
		m_marginBoxHeight(0),
		m_borderBoxWidth(0),
		m_borderBoxHeight(0),
		m_contentBoxWidth(0),
		m_contentBoxHeight(0),
		m_borderLeft(0),
		m_borderRight(0),
		m_borderTop(0),
		m_borderBottom(0),
		m_paddingLeft(0),
		m_paddingRight(0),
		m_paddingTop(0),
		m_paddingBottom(0),
		m_marginLeft(0),
		m_marginRight(0),
		m_marginTop(0),
		m_marginBottom(0),
		m_borderX(0),
		m_borderY(0),
		m_contentX(0),
		m_contentY(0),
		m_alignHorizontal(ALIGN_FLOAT),
		m_alignVertical(ALIGN_FLOAT),
		m_borderColor(sf::Color(102, 166, 255, 160)),
		m_backgroundColor(sf::Color(54, 31, 171, 160)),
		m_borderHoverColor(sf::Color(102, 166, 255, 200)),
		m_backgroundHoverColor(sf::Color(54, 31, 171, 200))
	{}

	void renderBox(UI &ui, sf::RenderTarget &renderTarget) 
	{
		sf::RectangleShape bbShape;
		sf::Color curBorderColor = m_hover ? m_borderHoverColor : m_borderColor;
		sf::Color curBgColor = m_hover ? m_backgroundHoverColor : m_backgroundColor;
		// draw borders
		if (m_borderTop != 0) {
			// top
			bbShape.setSize(sf::Vector2f(m_borderBoxWidth, m_borderTop));
			bbShape.setPosition(sf::Vector2f(m_allocation.bbX + m_borderX, m_allocation.bbY + m_borderY));
			bbShape.setOutlineThickness(0);
			bbShape.setFillColor(curBorderColor);
			renderTarget.draw(bbShape);
		}
		if (m_borderLeft != 0) {
			// left
			bbShape.setSize(sf::Vector2f(m_borderLeft, m_borderBoxHeight - m_borderTop - m_borderBottom));
			bbShape.setPosition(sf::Vector2f(m_allocation.bbX + m_borderX, m_allocation.bbY + m_borderY + m_borderTop));
			bbShape.setOutlineThickness(0);
			bbShape.setFillColor(curBorderColor);
			renderTarget.draw(bbShape);
		}
		if (m_borderRight != 0) {
			// right
			bbShape.setSize(sf::Vector2f(m_borderRight, m_borderBoxHeight - m_borderTop - m_borderBottom));
			bbShape.setPosition(sf::Vector2f(m_allocation.bbX + m_borderX + m_borderBoxWidth - m_borderRight, m_allocation.bbY + m_borderY + m_borderTop));
			bbShape.setOutlineThickness(0);
			bbShape.setFillColor(curBorderColor);
			renderTarget.draw(bbShape);
		}
		if (m_borderBottom != 0) {
			// bottom
			bbShape.setSize(sf::Vector2f(m_borderBoxWidth, m_borderBottom));
			bbShape.setPosition(sf::Vector2f(m_allocation.bbX + m_borderX, m_allocation.bbY + m_borderY + m_borderBoxHeight - m_borderBottom));
			bbShape.setOutlineThickness(0);
			bbShape.setFillColor(curBorderColor);
			renderTarget.draw(bbShape);
		}
		// draw content box
		bbShape.setSize(sf::Vector2f(m_contentBoxWidth + m_paddingLeft + m_paddingRight, m_contentBoxHeight + m_paddingTop + m_paddingBottom));
		bbShape.setPosition(sf::Vector2f(m_allocation.bbX + m_borderX + m_borderLeft, m_allocation.bbY + m_borderY + m_borderTop));
		bbShape.setOutlineThickness(0);
		bbShape.setFillColor(curBgColor);
		renderTarget.draw(bbShape);

		std::ostringstream ss;
		ss << this;
		sf::Text text(ss.str(), ui.getDefaultFont());
		text.setPosition(m_allocation.bbX + m_contentX + 2, m_allocation.bbY + m_contentY + 2);
		text.setCharacterSize(10);
		text.setColor(sf::Color(40, 40, 40, 120));
		renderTarget.draw(text);
	}

	virtual void render(UI &ui, sf::RenderTarget &renderTarget) = 0;

	UIElement &setMargin(int right, int left, int top, int bottom)
	{
		m_marginRight = right;
		m_marginLeft = left;
		m_marginTop = top;
		m_marginBottom = bottom;
		return *this;
	}

	UIElement &setPadding(int right, int left, int top, int bottom)
	{
		m_paddingRight = right;
		m_paddingLeft = left;
		m_paddingTop = top;
		m_paddingBottom = bottom;
		return *this;
	}

	UIElement &setBorder(int right, int left, int top, int bottom)
	{
		m_borderRight = right;
		m_borderLeft = left;
		m_borderTop = top;
		m_borderBottom = bottom;
		return *this;
	}

	UIElement &setSize(UISize width, UISize height)
	{
		m_boxWidth = width;
		m_boxHeight = height;
		return *this;
	}

	UIElement &setVerticalAlign(UIAlign align) {
		m_alignVertical = align;
		return *this;
	}

	UIElement &setHorizontalAlign(UIAlign align) {
		m_alignHorizontal = align;
		return *this;
	}

	// TODO setPosition
	UIElement &setPosition(UISize relX, UISize relY)
	{
		return *this;
	}

	virtual void setAllocation(UIBoundingBox const &newBB) 
	{
		m_allocation = newBB;
		std::cout << "setAllocation (" << this << ") ";
		newBB.dump(std::cout);
		std::cout << '\n';
		updateLayout();
	}

	virtual void updateMetrics(int containerWidth, int containerHeight) {
		calculateBoxSize(containerWidth, containerHeight);
	}
	
	virtual void updateLayout() {
		calculateBoxPosition();
	}

	virtual bool hitTest(int x, int y) const
	{
		if ((m_allocation.bbX + m_borderX < x) && 
			(x < (m_allocation.bbX + m_borderX + m_borderBoxWidth)) && 
			(m_allocation.bbY + m_borderY < y) && 
			(y < (m_allocation.bbY + m_borderY + m_borderBoxHeight))) { 
			return true;
		} else {
			return false;
		}
	}

	virtual void handleMouseMove(sf::Event const &event)
	{
		if (event.type == sf::Event::MouseMoved) {
			bool hit = hitTest(event.mouseMove.x, event.mouseMove.y);
			if (hit != m_hover) {
				std::cout << "handleMouseMove (" << this << ") mouse " << (hit ? "entering" : "leaving") << '\n';
			}
			m_hover = hit;
		}
	}

	virtual void onEvent(sf::Event const &event)
	{
		handleMouseMove(event);
	}


	void calculateBoxSize(int containerWidth, int containerHeight, int contentWidth = -1, int contentHeight = -1)
	{
		if (m_boxWidth == UISize::AUTO) {
			// size to container
			m_marginBoxWidth = containerWidth;
			m_borderBoxWidth = containerWidth - m_marginLeft - m_marginRight;
			m_contentBoxWidth = containerWidth - m_marginLeft - 
				m_borderLeft - m_paddingLeft - m_marginRight - 
				m_borderRight - m_paddingRight;
		} else if (m_boxWidth == UISize::ADJUST) {
			// size to content
			m_contentBoxWidth = contentWidth;
			m_borderBoxWidth = m_contentBoxWidth + m_paddingLeft + m_paddingRight + m_borderLeft + m_borderRight;
			m_marginBoxWidth = m_borderBoxWidth + m_marginLeft + m_marginRight;
		} else {
			// size border box
			m_borderBoxWidth = m_boxWidth.toPixels(containerWidth);
			m_marginBoxWidth = m_borderBoxWidth + m_marginLeft + m_marginRight;
			m_contentBoxWidth = m_borderBoxWidth - m_borderLeft - m_borderRight - 
				m_paddingLeft - m_paddingRight;
		}

		if (m_boxHeight == UISize::AUTO) {
			// size to container
			m_marginBoxHeight = containerHeight;
			m_borderBoxHeight = containerHeight - m_marginTop - m_marginBottom;
			m_contentBoxHeight = containerHeight - m_marginTop - 
				m_borderTop - m_paddingTop - m_marginBottom - 
				m_borderBottom - m_paddingBottom;
		} else if (m_boxHeight == UISize::ADJUST) {
			// size to content
			m_contentBoxHeight = contentHeight;
			m_borderBoxHeight = m_contentBoxHeight + m_paddingTop + m_paddingBottom + m_borderTop + m_borderBottom;
			m_marginBoxHeight = m_borderBoxHeight + m_marginTop + m_marginBottom;
		} else {
			// size border box
			m_borderBoxHeight = m_boxHeight.toPixels(containerHeight);
			m_marginBoxHeight = m_borderBoxHeight + m_marginTop + m_marginBottom;
			m_contentBoxHeight = m_borderBoxHeight - m_borderTop - m_borderBottom - 
				m_paddingTop - m_paddingBottom;
		}
	}


	void calculateBoxPosition()
	{
		// TODO alignment/fixed position
		if (m_alignHorizontal == ALIGN_FLOAT) {
			m_borderX = std::max(m_marginLeft, m_fixedX.toPixels(m_allocation.bbWidth));
		} else {
			m_borderX = alignPosition(m_borderBoxWidth, m_allocation.bbWidth, m_alignHorizontal);
		}

		if (m_alignVertical == ALIGN_FLOAT) {
			m_borderY = std::max(m_marginTop, m_fixedY.toPixels(m_allocation.bbHeight));
		} else {
			//std::cout << "borderBoxHeight=" << m_borderBoxHeight << " allocation.bbHeight=" << m_allocation.bbHeight << '\n';
			m_borderY = alignPosition(m_borderBoxHeight, m_allocation.bbHeight, m_alignVertical);
		}

		m_contentX = m_borderX + m_borderLeft + m_paddingLeft;
		m_contentY = m_borderY + m_borderTop + m_paddingTop;

		std::cout << "calculateBoxPosition (" << this << ") content=" << m_contentX << ',' << m_contentY << " border=" << m_borderX << ',' << m_borderY << '\n';
	}

	UIBoundingBox const &getAllocation() const {
		return m_allocation;
	}

	void getMarginBoxSize(int &marginBoxWidth, int &marginBoxHeight) const {
		marginBoxWidth = m_marginBoxWidth;
		marginBoxHeight = m_marginBoxHeight; 
	}

protected:
	//std::vector<UIEventHandler const *> m_eventHandlers;

	//============================
	// Position
	UISize m_fixedX;	// auto if floating
	UISize m_fixedY;	// auto if floating

	//============================
	// Border, padding & margin
	UISize m_boxWidth;
	UISize m_boxHeight;
	int m_marginBoxWidth;
	int m_marginBoxHeight;
	int m_borderBoxWidth;
	int m_borderBoxHeight;
	int m_contentBoxWidth;
	int m_contentBoxHeight;
	int m_borderLeft;
	int m_borderRight;
	int m_borderTop;
	int m_borderBottom;
	int m_paddingLeft;
	int m_paddingRight;
	int m_paddingTop;
	int m_paddingBottom;
	int m_marginLeft;
	int m_marginRight;
	int m_marginTop;
	int m_marginBottom;
	// relative to allocated space
	int m_borderX;
	int m_borderY;
	// relative to allocated space
	int m_contentX;
	int m_contentY;

	//============================
	// Alignment TODO
	UIAlign m_alignHorizontal;	// ignored if m_fixedX != AUTO
	UIAlign m_alignVertical;	// ignored if m_fixedY != AUTO

	bool m_adjustToChildren;
	bool m_updateLayout;

	//============================
	// Style TODO
	sf::Color m_borderColor;
	sf::Color m_backgroundColor;
	sf::Color m_borderHoverColor;
	sf::Color m_backgroundHoverColor;

	bool m_hover;

	// Bounding box
	UIBoundingBox m_allocation;
};

//====================================
// UIContainer
class UIContainer : public UIElement
{
public:
	virtual void addChild(UIElement *child) {
		m_children.push_back(child);
	}

	virtual void onEvent(sf::Event const &event)
	{
		handleMouseMove(event);
		for (auto child = m_children.begin(); 
			child != m_children.end();
			++child) {
			(*child)->onEvent(event);
		}
	}


protected:
	std::vector<UIElement *> m_children;
};


static inline int clamp(int value, int max) {
	return value > max ? max : value;
}


//====================================
// TextBox
// UI.textBox.defaultFont
// UI.textBox.defaultFontSize
class UITextBox : public UIElement
{
public:
	UITextBox(std::string const &str, sf::Font const &font, int characterSize = 14) : 
		m_string(str), 
		m_characterSize(characterSize),
		m_font(&font),
		m_textColor(sf::Color::Black)
	{
		m_boxWidth = UISize::ADJUST;
		m_boxHeight = UISize::ADJUST;
		m_backgroundColor = sf::Color::Transparent;
		m_backgroundHoverColor = sf::Color::Transparent;
		updateText();
		std::cout << "UITextBox (" << this << ") text=" << str.c_str() << '\n';
	}

	virtual void render(UI &ui, sf::RenderTarget &renderTarget)
	{
		renderBox(ui, renderTarget);
		renderTarget.draw(m_text);
	}

	void updateText()
	{
		m_text.setPosition(m_allocation.bbX + m_contentX, m_allocation.bbY + m_contentY);
		m_text.setString(m_string);
		m_text.setCharacterSize(m_characterSize);
		m_text.setFont(*m_font);
		m_text.setColor(m_textColor);
		m_textWidth = m_text.getLocalBounds().width;
		m_textHeight = m_text.getCharacterSize();
		std::cout << "UITextBox.updateText (" << this << ") width=" << m_textWidth << " height=" << m_textHeight << '\n';
	}

	virtual void updateMetrics(int containerWidth, int containerHeight) {
		calculateBoxSize(containerWidth, containerHeight, m_textWidth, m_textHeight);
	}

	UITextBox &setText(std::string const &str) {
		m_string = str;
		updateText();
		return *this;
	}

	std::string getText() const {
		return m_string;
	}

	UITextBox &setFont(sf::Font const &font) {
		m_font = &font;
		updateText();
		return *this;
	}

	sf::Font const &getFont() const {
		return *m_font;
	}

	sf::Color const &getTextColor() const {
		return m_textColor;
	}

	UITextBox &setTextColor(sf::Color const &color) {
		m_textColor = color;
		updateText();
		return *this;
	}

	virtual void updateLayout() 
	{
		calculateBoxPosition();
		updateText();
	}

private:
	std::string m_string;
	int m_characterSize;
	int m_textWidth;
	int m_textHeight;
	sf::Font const *m_font;
	sf::Text m_text;
	sf::Color m_textColor;
};

//====================================
// UIButton
class UIButton : public UIElement
{
public:
	UIButton(std::string const &str, sf::Font const &font, int characterSize = 20) : 
		m_textBox(str, font, characterSize)
	{
		setBorder(2,2,2,2);
		setPadding(2,2,2,2);
		m_borderColor = sf::Color(102, 166, 255, 255);
		m_backgroundColor = sf::Color(41, 201, 255, 255);
		m_borderHoverColor = sf::Color(102, 166, 255, 255);
		m_backgroundHoverColor = sf::Color(130, 224, 255, 255);
		m_textBox.setHorizontalAlign(ALIGN_CENTER);
		m_textBox.setVerticalAlign(ALIGN_CENTER);
	}

	virtual void render(UI &ui, sf::RenderTarget &renderTarget) {
		renderBox(ui, renderTarget);
		m_textBox.render(ui, renderTarget);
	}

	UITextBox &getLabel() {
		return m_textBox;
	}

	virtual void updateMetrics(int layoutBoxWidth, int layoutBoxHeight)
	{
		std::cout << "UIButton.updateMetrics (" << this << ")\n";
		m_textBox.updateMetrics(layoutBoxWidth, layoutBoxHeight);
		int textBoxWidth, textBoxHeight;
		m_textBox.getMarginBoxSize(textBoxWidth, textBoxHeight);
		calculateBoxSize(layoutBoxWidth, layoutBoxHeight, textBoxWidth, textBoxHeight);
	}

	virtual void updateLayout() 
	{
		calculateBoxPosition();
		m_textBox.setAllocation(UIBoundingBox(m_allocation.bbX + m_contentX, m_allocation.bbY + m_contentY, m_contentBoxWidth, m_contentBoxHeight));
	}

	/*virtual void updateLayout()
	{}*/

protected:
	UITextBox m_textBox;
};

enum UIOrientation
{
	VERTICAL,
	HORIZONTAL
};

/*class UISplitPanel : public UIContainer
{
public:
	void addSplit(UISize const &position) {
		m_splitPositions.add
	}

private:
	std::vector<UISize> m_splitPositions;
	UIOrientation m_orientation;
};
*/

class UIColumnContainer : public UIContainer
{
public:
	UIColumnContainer() : UIContainer()
	{
		setBorder(0, 0, 0, 0);
		setPadding(0, 0, 0, 0);
		//m_backgroundHoverColor = m_backgroundColor;
		//m_borderHoverColor = m_borderColor;
	}

	virtual void updateMetrics(int layoutBoxWidth, int layoutBoxHeight)
	{
		std::cout << "UIColumnContainer.updateMetrics (" << this << ")\n";
		// update box metrics
		calculateBoxSize(layoutBoxWidth, layoutBoxHeight);
		// update maximum widget height
		m_maxChildHeight = m_contentBoxHeight / m_children.size();
		// update metrics of child elements
		for (auto child = m_children.begin(); child != m_children.end(); ++child)
		{
			(*child)->updateMetrics(m_contentBoxWidth, m_maxChildHeight);
		}
	}

	virtual void updateLayout() 
	{
		calculateBoxPosition();
		int relY = 0;
		int stackHeightFree = m_contentBoxHeight; 
		for (auto i = m_children.begin(); i != m_children.end(); ++i)
		{
			UIElement *child = *i;
			UIBoundingBox childBB;
			int childWidth, childHeight;
			child->getMarginBoxSize(childWidth, childHeight);
			childBB.bbWidth = m_contentBoxWidth;
			childBB.bbHeight = m_maxChildHeight;
			childBB.bbX = m_allocation.bbX + m_contentX;
			childBB.bbY = m_allocation.bbY + m_contentY + relY;
			relY += childBB.bbHeight;
			child->setAllocation(childBB);
		}
	}

	virtual void render(UI &ui, sf::RenderTarget &renderTarget) {
		renderBox(ui, renderTarget);
		for (auto child = m_children.begin(); child != m_children.end(); ++child)
		{
			(*child)->render(ui, renderTarget);
		}
	}

private:
	int m_maxChildHeight;
};

void UI::updateLayout()
{
	if (m_root != NULL) {
		UIBoundingBox rootBB(0, 0, m_width, m_height);
		m_root->updateMetrics(m_width, m_height);
		m_root->setAllocation(rootBB);
	}
	m_doLayout = false;
}

void UI::render(sf::RenderTarget &renderTarget) {
	if (m_doLayout) {
		updateLayout();
	}
	m_root->render(*this, renderTarget);
}

void UI::onEvent(sf::Event const &event)
{
	m_root->onEvent(event);
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML OpenGL", sf::Style::Default, sf::ContextSettings(32, 0, 8));
	window.setFramerateLimit(0);
	window.setVerticalSyncEnabled(true);

	/*Window window;
	UIColumnContainer column;
	column.setSize(UI::Dimension(FIXED, 300));
	column.setLayout(UI::ALIGN_RIGHT);
	window.addEventHandler(column);*/

	UI ui(800, 600);
	UIColumnContainer columnContainer;
	UIButton button1("hello", ui.getDefaultFont()), 
		button2("world", ui.getDefaultFont()),
		button3("three", ui.getDefaultFont()), 
		button4("four", ui.getDefaultFont());
	button1.setSize(UISize::percentage(50), UISize::ADJUST).setHorizontalAlign(ALIGN_CENTER).setVerticalAlign(ALIGN_CENTER);
	button2.setSize(UISize::percentage(50), UISize::ADJUST).setHorizontalAlign(ALIGN_CENTER).setVerticalAlign(ALIGN_CENTER);
	button3.setSize(UISize::AUTO, UISize::ADJUST).setHorizontalAlign(ALIGN_LEFT).setVerticalAlign(ALIGN_CENTER);
	button4.setSize(UISize::percentage(50), UISize::ADJUST).setHorizontalAlign(ALIGN_RIGHT).setVerticalAlign(ALIGN_CENTER);
	columnContainer.addChild(&button1);
	columnContainer.addChild(&button2);
	columnContainer.addChild(&button3);
	columnContainer.addChild(&button4);
	columnContainer.setBorder(1,1,1,1);
	columnContainer.setSize(UISize::px(200), UISize::AUTO);
	ui.setRoot(&columnContainer);
	UITextBox tb("Hello world!", ui.getDefaultFont(), 10); 
	columnContainer.addChild(&tb);
	tb.setHorizontalAlign(ALIGN_CENTER).setVerticalAlign(ALIGN_CENTER).setPadding(2, 2, 2, 2);

	/*UISplitPanel split;

	UIRowPanel rowPanel;
	rowPanel.setElementAlign(LEFT);
	rowPanel.setElementPadding(2);
	rowPanel.add(button1);
	rowPanel.add(button2);
	rowPanel.add(button3);

	split.add(rowPanel, BOTTOM, UISize::percentage(10));*/

    // run the program as long as the window is open
    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event))
        {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed) {
                window.close();
				break;
			} else if (event.type == sf::Event::Resized) {
				// window resize: update 2D projection matrix
				window.setView(sf::View(sf::FloatRect(0.f, 0.f, static_cast<float>(event.size.width), static_cast<float>(event.size.height))));
				ui.setSize(event.size.width, event.size.height);
			} else if (event.type == sf::Event::MouseMoved) {
				std::cout << "MouseMoved " << event.mouseMove.x << ',' << event.mouseMove.y << '\n';
				// TODO UI.mouseMoved
			} else if (event.type == sf::Event::MouseButtonPressed) {
				std::cout << "MouseButtonPressed " << event.mouseButton.x << ',' << event.mouseButton.y << ':' << event.mouseButton.button << '\n';
			} else if (event.type == sf::Event::MouseButtonReleased) {
				std::cout << "MouseButtonReleased " << event.mouseButton.x << ',' << event.mouseButton.y << ':' << event.mouseButton.button << '\n';
			}
			ui.onEvent(event);
        }

		window.clear();
		ui.render(window);
		window.display();
    }

    return 0;
}
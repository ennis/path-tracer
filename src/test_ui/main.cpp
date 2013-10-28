#include <GL/glew.h>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <sstream>
#include <iostream>
#include <functional>
#include <memory>

#include "ui.hpp"
#include "panel.hpp"
#include "button.hpp"
#include "checkbox.hpp"
#include "spacer.hpp"

#define glCheck(call) { call; glCheckError(#call); }

static const unsigned int WIDTH = 800;
static const unsigned int HEIGHT = 600;

void glCheckError(char const * call)
{
	int err = glGetError();
	if (err != GL_NO_ERROR) {
		std::clog << call << " -> " << err << '\n';
	}
}

static inline int clamp(int value, int max) {
	return value > max ? max : value;
}

ui::Button::Ptr createSmallButton(std::string label)
{
	ui::Button::Ptr b = ui::Button::create(label);
	return b;
}

int main()
{
	using namespace sf;
	using namespace ui;

    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML OpenGL", sf::Style::Default, sf::ContextSettings(32, 0, 8));
	window.setFramerateLimit(0);
	window.setVerticalSyncEnabled(true);

	sf::Font defaultFont;
	defaultFont.loadFromFile("uzura.ttf");

	auto engine = DefaultEngine::create(defaultFont);
	auto ui = UI::create(engine);
	ui->setSize(WIDTH, HEIGHT);

	auto panel = Panel::create();
	auto subpanel1 = Panel::create(Panel::HORIZONTAL, Panel::NO_SPACING);
	auto subpanel2 = Panel::create(Panel::HORIZONTAL, Panel::NO_SPACING);
	auto button1 = Button::create("Button 1");
	auto button2 = Button::create("Button 2");
	auto button3 = Button::create("Button 3");
	
	auto button41 = Button::create("Button 4|1");
	auto button42 = Button::create("Button 4|2");
	auto button43 = Button::create("Button 4|3");
	
	auto button51 = Button::create("Button 5|1");
	auto button52 = Button::create("Button 5|2");
	auto button53 = Button::create("Button 5|3");

	auto textbox = TextBox::create("Hello world!");
	auto checkbox = CheckBox::create();
	
	/*button1->setPlacement(Element::CC);
	button2->setPlacement(Element::TR);
	button3->setPlacement(Element::BL);*/

	//subpanel1->setHeight(30);
	subpanel1->add(button41);
	subpanel1->add(button42);
	//subpanel1->add(button43);
	
	//subpanel2->setHeight(30);
	subpanel2->add(button51);
	subpanel2->add(button52);
	//subpanel2->add(button43);

	//panel->add(subpanel1);
	//panel->add(subpanel2);
	panel->add(button1);
	panel->add(button2);
	panel->add(button3);
	panel->add(Spacer::create());
	panel->add(checkbox);
	panel->add(textbox);
	//panel->setWidth(ui::Size::px(200));

	button1->setSize(Size(Size::FILL, Size::ADJUST));
	panel->setSize(Size(200, Size::FILL));

	ui->add(panel);
	int numClicks = 0;

	button1->onClickEvent.add([&](Element::Ptr, int, int) { std::cout << "Button clicked!\n"; });
	panel->onHoverEnterEvent.add([](Element::Ptr) { std::cout << "Hover enter\n"; });
	panel->onHoverLeaveEvent.add([](Element::Ptr) { std::cout << "Hover exit\n"; });
	checkbox->onClickEvent.add([&checkbox, &textbox, &numClicks](Element::Ptr, int, int) { 
		std::cout << "CheckBox clicked, checked=" << checkbox->isChecked() << "\n";
		textbox->setText("Clicks: " + std::to_string(++numClicks));
	});

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
				ui->setSize(event.size.width, event.size.height);
			} else if (event.type == sf::Event::MouseMoved) {
				//std::cout << "MouseMoved " << event.mouseMove.x << ',' << event.mouseMove.y << '\n';
				// TODO UI.mouseMoved
			} else if (event.type == sf::Event::MouseButtonPressed) {
				//std::cout << "MouseButtonPressed " << event.mouseButton.x << ',' << event.mouseButton.y << ':' << event.mouseButton.button << '\n';
			} else if (event.type == sf::Event::MouseButtonReleased) {
				//std::cout << "MouseButtonReleased " << event.mouseButton.x << ',' << event.mouseButton.y << ':' << event.mouseButton.button << '\n';
			}
			ui->onEvent(event);
        }

		window.clear();
		ui->render(window);
		window.display();
    }

    return 0;
}

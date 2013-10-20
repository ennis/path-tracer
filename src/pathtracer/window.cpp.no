#include <cassert>
#include <iostream>

#include "window.hpp"

void Window::create(int width, int height, bool fullscreen)
{
	m_renderWindow.create(sf::VideoMode(width, height, 24), "Render window");
	m_drawRect.setSize(sf::Vector2f(static_cast<float>(width), static_cast<float>(height))); 
	m_drawRect.setPosition(0.f, 0.f);
}

void Window::setFilm(Film const *film)
{
	m_film = film;
	if (m_filmPixelData != NULL) {
		std::cout << "Window.setFilm: NULL\n";
		delete [] m_filmPixelData;
	}
	if (film != NULL) {
		std::cout << "Window.setFilm: width=" << film->getWidth() << " height=" << film->getHeight() << '\n';
		int texWidth = film->getWidth();
		int texHeight = film->getHeight();
		m_drawRect.setTexture(NULL);
		m_texture.create(texWidth, texHeight);
		m_drawRect.setTexture(&m_texture, true);
		m_filmPixelData = new uint32_t[texWidth * texHeight];
	}
}

void Window::updateImage()
{
	assert(m_film != NULL);
	assert(m_tempPixelData != NULL);
	m_film->convertToRGB(m_filmPixelData);
	sf::Vector2u texSize = m_texture.getSize();
	m_texture.update(reinterpret_cast<sf::Uint8*>(m_filmPixelData), texSize.x, texSize.y, 0, 0);
	std::cout << "updateImage\n";
}

void Window::mainLoop()
{
	while (m_renderWindow.isOpen())
    {
		//
		// Event loop
		//
        sf::Event event;
        while (window.pollEvent(event))
        {
			//
			// Close on escape key and close button
			//
            if (event.type == sf::Event::Closed) {
                window.close();
				if (renderParameters.progressive) {
					renderer.stop();
				}
				break;
			}
			else if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::Escape) {
					window.close();
					if (renderParameters.progressive) {
						renderer.stop();
					}
					break;
				}
			} else if (event.type == sf::Event::MouseButtonPressed) {
				sf::Vector2i pos = sf::Mouse::getPosition(window);
				renderParameters.debugPixelX = pos.x;
				renderParameters.debugPixelY = pos.y;
				std::clog << "Debug pixel " << pos.x << ", " << pos.y << "\n";
			}
		}
	}
}
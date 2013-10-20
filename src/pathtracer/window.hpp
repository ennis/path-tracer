#pragma once
#include "vec.hpp"
#include "camera.hpp"
#include "film.hpp"

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

class WindowEventHandler
{
public:
	virtual void onClose() = 0;
	virtual void onEvent(sf::Event const &event) = 0;
	virtual void onDraw() = 0;
};

class Window
{
public:
	Window() :
		m_drawRect(),
		m_texture(),
		m_renderWindow(),
		m_camera(NULL),
		m_filmPixelData(NULL)
	{}

	~Window() {
		if (m_filmPixelData) {
			delete [] m_filmPixelData;
		}
	}

	void create(int width, int height, bool fullscreen);

	void beginGL();
	void endGL();

	void begin2D();
	void end2D();

	void setFilm(Film const *film);

	void updateImage();
	void updateCamera(Camera const &camera);

	void resize(int width, int height);

	void setTitle(char const *title) {
		m_renderWindow.setTitle(std::string(title));
	}

	void mainLoop();

private:
	void onResize(int width, int height);
	void onClick(int x, int y, int mouseButton);
	void onClose();
	
	// drawing area
	sf::RectangleShape m_drawRect;
	// target texture
	sf::Texture m_texture;
	// render window
	sf::RenderWindow m_renderWindow;
	Camera const *m_camera;
	Film const *m_film;
	uint32_t *m_filmPixelData;
};
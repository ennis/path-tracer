#pragma once
#include "vec.hpp"
#include "camera.hpp"

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

class Window
{
public:

	void create(int width, int height, bool fullscreen);

	void beginGL();
	void endGL();

	void begin2D();
	void end2D();

	void updateImage();
	void updateCamera(Camera const &camera);

	void resize(int width, int height);

private:
	void onResize(int width, int height);
	void onClick(int x, int y, int mouseButton);

	sf::RenderWindow m_renderWindow;
};
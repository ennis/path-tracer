#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <thread>
#include <atomic>
#include <sstream>
#include <ctime>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include "vec.hpp"
#include "ray.hpp"
#include "camera.hpp"
#include "path.hpp"
#include "primitive.hpp"
#include "scene.hpp"
#include "mesh.hpp"

using namespace std;

static const unsigned int XRES = 640;
static const unsigned int YRES = 480;
static const float ASPECT_RATIO = 
	static_cast<float>(XRES) / static_cast<float>(YRES);

sf::Texture texture;
Film *film;
PathRenderer renderer;
RenderParameters renderParameters;

template <typename T>
T clamp(T v, T min, T max)
{
	return (v < min) ? min : ((v > max) ? max : v);
}

//===============================
// Render thread
void work()
{	
	// render parameters
	renderParameters.samplesPerPixel = 50;
	renderParameters.maxDepth = 8;
	renderParameters.supersampling = true;
	renderParameters.cosineWeightedSampling = false;
	renderParameters.directLightingOnly = true;
	renderParameters.explicitLightSampling = true;
	renderParameters.pixelWidth = XRES;
	renderParameters.pixelHeight = YRES;
	renderParameters.progressive = true;

	// BSDFs
	LambertianBRDF lambertian;
	MirrorBRDF mirror(0.5f);
	PhongBRDF phong1000(10000, Vec(1.f, 1.f, 1.f));
	PhongBRDF phong100(100, Vec(1.f, 1.f, 1.f));

	// Environment map
	EnvironmentMap glacier;
	glacier.loadFromFile("uffizi-large.hdr", 2.f);

	// Textures
	CheckerboardTexture checkerboard(
		Vec(0.1f, 0.1f, 0.05f), 
		Vec(1.f, 1.f, 1.f),
		0.5f,
		0.5f);

	ColorTexture redTexture(Vec(0.9f, 0.15f, 0.05f));
	ColorTexture white(Vec(1.f, 1.f, 1.f));
	ColorTexture blue(Vec(0.2f, 0.4f, 1.f));

	// Primitives
	Sphere S(
		Point(0.f, 0.f, 0.f),
		0.7f,
		NULL,
		&blue,
		&lambertian,
		Vec(0.f, 0.f, 0.f));
	
	Sphere S2(
		Point(1.f, 1.f, 0.5f),
		1.f,
		NULL,
		&white,
		&phong1000,
		Vec(0.f, 0.f, 0.f));

	Sphere SL(
		Point(-0.5f, 1.f, 2.f),
		0.7f,
		NULL,
		&white,
		&lambertian,
		Vec(4.f, 4.f, 4.f));


	Plane P(
		Point(0.f, -2.f, 0.f),
		Vec(0.f,1.f,0.f),
		NULL,
		&redTexture,
		&lambertian,
		Vec(0.f, 0.f, 0.f));

	
	// Meshes
	Mesh mesh(
		NULL,
		&blue,
		&lambertian,
		Vec(0.f, 0.f, 0.f));
	mesh.loadFromFile("cornellbox_triangles.obj");

	// Camera
	// TODO debug screenDist
	Camera camera(
		Point(1.5f, 1.5f, 3.5f), 
		Point(-1.0f, 1.0f, 0.f),
		ASPECT_RATIO, 
		1.f);

	// Scene
	Scene scene;
	scene.setCamera(&camera);
	scene.setAmbient(Vec(0.0f, 0.0f, 0.0f));
	//scene.setEnvironmentMap(&glacier);
	//scene.add(&S);
	//scene.add(&S2);
	scene.addAreaLight(&SL);
	//scene.add(&P);
	scene.add(&mesh);

	// Render
	renderer.render(scene, *film, renderParameters);

}

//===============================
// main
int main(int argc, char ** argv)
{
	//
	// Create render window
	//
	sf::RenderWindow window(
		sf::VideoMode(XRES, YRES), 
		"SFML OpenGL", 
		sf::Style::Default, 
		sf::ContextSettings(32, 0, 8));
	
	//
	// Rectangle to draw the image into
	//
	sf::RectangleShape drawRect(
		sf::Vector2f(static_cast<float>(XRES), static_cast<float>(YRES)));

	//
	// Bind the texture
	//
	texture.create(XRES, YRES);
	drawRect.setTexture(&texture);

	film = new Film(XRES, YRES);
	uint32_t *textureData = new uint32_t[XRES*YRES];

	sf::Clock clock;	// clock to measure SPS
	long delta = 0;
	unsigned int samplesPerSecond = 0;
	unsigned int lastSamples = 0;

	sf::Font font;
	if (!font.loadFromFile("uzura.ttf")) {
		std::cerr << "font.loadFromFile failed\n";
		return EXIT_FAILURE;
	}

	sf::Text spsText;
	spsText.setFont(font);
	spsText.setColor(sf::Color(255, 255, 255, 255));
	spsText.setPosition(10.f, 38.f);
	
	sf::Text samplesText;
	samplesText.setFont(font);
	samplesText.setColor(sf::Color(255, 255, 255, 255));
	samplesText.setPosition(10.f, 10.f);

	//
	// launch render thread
	//
	std::thread worker(&work);


    while (window.isOpen())
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
			}
		}

		if (renderer.isStarted())
		{

			//
			// Draw current film
			//
			window.clear(sf::Color::Red);
			window.draw(drawRect);

			if (!renderer.isFinished()) {
				//
				// Draw text
				//
				if (renderParameters.progressive) {
					std::stringstream ss;
					ss << renderer.getNumFrames();
					samplesText.setString(ss.str());
					window.draw(samplesText);
				}

				if (delta != 0) {
					std::stringstream ss;
					ss << samplesPerSecond;
					ss << " SPS";
					spsText.setString(ss.str());
					window.draw(spsText);
				}
			}
		
			//
			// Update display
			//
			window.display();

			
			if (!renderer.isFinished()) {
				//
				// update texture every 250ms
				//
				delta = clock.getElapsedTime().asMilliseconds();
				if (delta >= 250) 
				{
					film->convertToRGB(textureData);
					texture.update((sf::Uint8*)textureData, XRES, YRES, 0, 0);
					unsigned int curSamples = renderer.getNumSamples();
					samplesPerSecond = (curSamples - lastSamples)*1000/delta;
					lastSamples = curSamples;
					clock.restart();
				}
			}
		} 
		else {
			//
			// Not yet started
			//
			window.clear(sf::Color::Red);
		}
	}

	worker.join();

	//
	// Save image to output_<random>.png
	//
	std::stringstream renderFileName("output_");
	renderFileName << "output_" << time(NULL) << ".png";
	std::string str = renderFileName.str();
	std::clog << "\nSaving image to " << str.c_str() << "...";
	texture.copyToImage().saveToFile(str.c_str());
	std::clog << "Done\n";

	//
	// Cleanup
	//
	delete[] textureData;
	delete film;

	return 0;
}


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

#include "sphere.hpp"
#include "vec.hpp"
#include "ray.hpp"
#include "matrix4x4.hpp"
#include "camera.hpp"
#include "renderer.hpp"
#include "plane.hpp"
#include "triangle.hpp"
#include "glass.hpp"
#include "diffuse.hpp"
#include "mirror.hpp"
#include "specular.hpp"
#include "schlick.hpp"
#include "phong.hpp"
#include "ashikhmin_shirley.hpp"

using namespace std;

static const unsigned int XRES = 320;
static const unsigned int YRES = 240;
static const float ASPECT_RATIO = static_cast<float>(XRES)/static_cast<float>(YRES);

sf::Texture texture;
Film *film;
Renderer *renderer;

template<typename T>
T clamp(T v, T min, T max)
{
	return (v < min) ? min : ((v > max) ? max : v);
}

//
// Basic primitives
//
//
//
GeometricPrimitive* createSphere(Point const& pos, float radius, Material const* mat)
{
	return new GeometricPrimitive(new Sphere(translate(pos), radius), mat);
}

GeometricPrimitive* createPlane(Point const& pos, Vec const& normal, Material const* mat)
{
	return new GeometricPrimitive(new Plane(normal, pos), mat);
}

//
// Materials
//
//
Material const* createGlassMaterial(float refractionIndex, Vec const& surfaceColor)
{
	return new Material(new ColorTexture(surfaceColor), nullVec, false, new GlassBSDF(refractionIndex));
}

Material const* createMirrorMaterial(Vec const& surfaceColor)
{
	return new Material(new ColorTexture(surfaceColor), nullVec, false, new MirrorBSDF());
}

Material const* createDiffuseMaterial(Vec const& surfaceColor)
{
	return new Material(new ColorTexture(surfaceColor), nullVec, false, new DiffuseBSDF());
}

Material const* createGlossyMaterial(Vec const& surfaceColor, float R0)
{
	return new Material(new ColorTexture(surfaceColor), nullVec, false, new SchlickBSDF(R0));
}

Material const* createLightMaterial(Vec const& emittance) 
{
	return new Material(new ColorTexture(nullVec), emittance, true, new DiffuseBSDF());
}


//
// Render thread
//
//
void work()
{	
	RenderParameters params;
	params.samplesPerPixel = 50;
	params.maxDepth = 5;
	params.supersampling = true;
	params.cosineWeightedSampling = false;
	params.directLightingOnly = false;
	params.explicitLightSampling = true;
	params.pixelWidth = XRES;
	params.pixelHeight = YRES;

	Camera cam(Point(0.f, 2.f, -3.5f), Point(0.f, 0.f, 0.f), ASPECT_RATIO, 1.f);

	Material const* glass110 = createGlassMaterial(1.10f, Vec(1.f, 1.f, 1.f));
	Material const* mirror = createMirrorMaterial(Vec(1.f, 1.f, 1.f));
	Material const* greenDiffuse = createDiffuseMaterial(Vec(0.f, 1.f, 0.f));
	Material const* greenSchlickMat = createGlossyMaterial(Vec(0.7f, 1.f, 0.7f), 0.03f);
	Material const* diffuseWallDownMat = createDiffuseMaterial(Vec(0.2f, 0.9f, 0.2f));
	Material const* lightMat = createLightMaterial(Vec(10.f, 10.f, 10.f));

	GeometricPrimitive const* sphere = createSphere(Point(0.f, 0.f, 0.f), 1.f, greenSchlickMat);
	GeometricPrimitive const* wallDown = createPlane(Point(0.f, -1.3f, 0.f), Vec(0.f, 1.f, 0.f), diffuseWallDownMat);
	GeometricPrimitive const* lightSource = createSphere(Point(0.f, 10.f, 2.f), 3.f, lightMat);

	std::vector<Primitive const*> scene;
	scene.push_back(sphere);
	scene.push_back(wallDown);
	scene.push_back(lightSource);
	
	renderer->render(params, cam, scene, *film);
}

int main(int argc, char ** argv)
{
	//
	// Create render window
	//
	sf::RenderWindow window(sf::VideoMode(XRES, YRES), 
							"SFML OpenGL", 
							sf::Style::Default, 
							sf::ContextSettings(32, 0, 8));
	
	//
	// Rectangle to draw the image into
	//
	sf::RectangleShape drawRect(sf::Vector2f(static_cast<float>(XRES), static_cast<float>(YRES)));

	//
	// Bind the texture
	//
	texture.create(XRES, YRES);
	drawRect.setTexture(&texture);

	film = new Film(XRES, YRES);
	renderer = new Renderer;
	uint32_t *textureData = new uint32_t[XRES*YRES];

	std::stringstream sps_str("??? SPS");
	sf::Clock clock;	// clock to measure SPS
	unsigned int lastSamples = 0;

	sf::Font font;
	if (!font.loadFromFile("uzura.ttf")) {
		std::cerr << "font.loadFromFile failed\n";
		return EXIT_FAILURE;
	}
	
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
				break;
			}
			else if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::Escape) {
					window.close();
					break;
				}
			}
		}

		if (renderer->isStarted())
		{

			//
			// Draw current film
			//
			window.clear(sf::Color::Red);
			window.draw(drawRect);

			if (!renderer->isFinished()) {
				//
				// Draw text
				//
				sf::Text sps_text(sps_str.str(), font);
				sps_text.setColor(sf::Color(255, 255, 255, 255));
				sps_text.setPosition(10.f, 38.f);
				window.draw(sps_text);
			}
		
			//
			// Update display
			//
			window.display();

			
			if (!renderer->isFinished()) {
				//
				// update texture every 250ms
				//
				long delta = clock.getElapsedTime().asMilliseconds();
				if (delta >= 250) 
				{
					film->convertToRGB(textureData);
					texture.update((sf::Uint8*)textureData, XRES, YRES, 0, 0);
					unsigned int curSamples = renderer->getNumSamples();
					sps_str.str("");
					sps_str << (curSamples - lastSamples)*1000/delta;
					sps_str << " SPS";
					clock.restart();
					lastSamples = curSamples;
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
	std::stringstream file_name("output_");
	file_name << "output_" << time(NULL) << ".png";
	std::string str = file_name.str();

	std::clog << "\nSaving image to " << str.c_str() << "...";
	texture.copyToImage().saveToFile(str.c_str());
	std::clog << "Done\n";

	//
	// Cleanup
	//
	delete[] textureData;
	delete film;
	delete renderer;

	return 0;
}


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

static const unsigned int XRES = 640;
static const unsigned int YRES = 480;
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
	params.maxDepth = 8;
	params.supersampling = true;
	params.cosineWeightedSampling = false;
	params.directLightingOnly = false;
	params.explicitLightSampling = true;
	params.pixelWidth = XRES;
	params.pixelHeight = YRES;
	params.progressive = true;

	Camera cam(Point(0.f, 2.f, -4.5f), Point(0.f, 0.f, 0.f), ASPECT_RATIO, 1.f);

	Material const* glass110 = createGlassMaterial(1.10f, Vec(1.f, 1.f, 1.f));
	Material const* mirror = createMirrorMaterial(Vec(1.f, 1.f, 1.f));
	Material const* greenDiffuse = createDiffuseMaterial(Vec(0.f, 1.f, 0.f));
	Material const* greenSchlickMat = createGlossyMaterial(Vec(0.2f, 1.f, 0.7f), 0.2f);
	//Material const* diffuseWallDownMat = createDiffuseMaterial(Vec(0.2f, 0.9f, 0.2f));
	Material const* lightMat = createLightMaterial(Vec(10.f, 10.f, 8.f));

	Texture const* checkerboard = new CheckerboardTexture(Vec(0.f, 0.f, 0.f), Vec(1.0f, 1.0f, 1.0f), 1.f, 1.f);
	Material const* diffuseWallDownMat = new Material(checkerboard, nullVec, false, new DiffuseBSDF());
	Material const* sphereMat = new Material(new ColorTexture(Vec(1.f, 1.f, 0.5f)), nullVec, false, new SchlickBSDF(0.10f));

	GeometricPrimitive const* sphere = createSphere(Point(0.f, 0.f, 1.f), 1.f, glass110);
	GeometricPrimitive const* wallDown = createPlane(Point(0.f, -2.f, 0.f), Vec(0.f, 1.f, 0.f), diffuseWallDownMat);
	GeometricPrimitive const* lightSource = createSphere(Point(0.f, 10.f, 2.f), 3.f, lightMat);

	Geometry const* ellipsoidGeom = new Sphere(scale(Vec(1.35f, 1.15f, 1.f)), 1.f);
	GeometricPrimitive const* ellipsoid = new GeometricPrimitive(ellipsoidGeom, sphereMat);

	std::vector<Primitive const*> scene;
	scene.push_back(ellipsoid);
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

	
	sf::Clock clock;	// clock to measure SPS
	long delta = 0;
	unsigned int samplesPerSecond = 0;
	unsigned int lastSamples = 0;

	sf::Font font;
	if (!font.loadFromFile("uzura.ttf")) {
		std::cerr << "font.loadFromFile failed\n";
		return EXIT_FAILURE;
	}

	sf::Text sps_text;
	sps_text.setFont(font);
	sps_text.setColor(sf::Color(255, 255, 255, 255));
	sps_text.setPosition(10.f, 38.f);
	
	sf::Text samples_text;
	samples_text.setFont(font);
	samples_text.setColor(sf::Color(255, 255, 255, 255));
	samples_text.setPosition(10.f, 10.f);
	
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
				if (renderer->getParameters().progressive) {
					renderer->stopProgressive();
				}
				break;
			}
			else if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::Escape) {
					window.close();
					if (renderer->getParameters().progressive) {
						renderer->stopProgressive();
					}
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
				if (renderer->getParameters().progressive) {
					std::stringstream ss;
					ss << renderer->getNumFrames();
					samples_text.setString(ss.str());
					window.draw(samples_text);
				}

				if (delta != 0) {
					std::stringstream ss;
					ss << samplesPerSecond;
					ss << " SPS";
					sps_text.setString(ss.str());
					window.draw(sps_text);
				}
			}
		
			//
			// Update display
			//
			window.display();

			
			if (!renderer->isFinished()) {
				//
				// update texture every 250ms
				//
				delta = clock.getElapsedTime().asMilliseconds();
				if (delta >= 250) 
				{
					film->convertToRGB(textureData);
					texture.update((sf::Uint8*)textureData, XRES, YRES, 0, 0);
					unsigned int curSamples = renderer->getNumSamples();
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


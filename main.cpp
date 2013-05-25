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

static const unsigned int XRES = 300;
static const unsigned int YRES = 300;
static const float ASPECT_RATIO = static_cast<float>(XRES)/static_cast<float>(YRES);

sf::Texture texture;

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
GeometricPrimitive* createSphere(Vec const& pos, float radius, Material const* mat)
{
	return new GeometricPrimitive(new Sphere(MTranslate(pos), MTranslate(-pos), radius), mat);
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
	return new Material(new ColorTexture(surfaceColor), Vec::null, false, new GlassBSDF(refractionIndex));
}

Material const* createMirrorMaterial(Vec const& surfaceColor)
{
	return new Material(new ColorTexture(surfaceColor), Vec::null, false, new MirrorBSDF());
}

Material const* createDiffuseMaterial(Vec const& surfaceColor)
{
	return new Material(new ColorTexture(surfaceColor), Vec::null, false, new DiffuseBSDF());
}

Material const* createGlossyMaterial(Vec const& surfaceColor, float R0)
{
	return new Material(new ColorTexture(surfaceColor), Vec::null, false, new SchlickBSDF(R0));
}

Material const* createLightMaterial(Vec const& emittance) 
{
	return new Material(new ColorTexture(Vec::null), emittance, true, new DiffuseBSDF());
}


//
// Render thread
//
//
void work()
{	
	RenderParameters params;
	params.samplesPerPixel = 40;
	params.maxDepth = 5;
	params.supersampling = true;
	params.cosineWeightedSampling = false;
	params.directLightingOnly = false;
	params.explicitLightSampling = true;
	params.pixelWidth = XRES;
	params.pixelHeight = YRES;

	Camera cam(Point(0, -1.5, -3.0f), Point(0.f, -3.f, 0.f), ASPECT_RATIO,  1.f);

	Material const* glass110 = createGlassMaterial(1.10f, Vec(1.f, 1.f, 1.f));
	Material const* mirror = createMirrorMaterial(Vec(1.f, 1.f, 1.f));
	Material const* greenDiffuse = createDiffuseMaterial(Vec(0.f, 1.f, 0.f));
	Material const* greenSchlickMat = createGlossyMaterial(Vec(0.7f, 1.f, 0.7f), 0.08f);
	Material const* diffuseWallDownMat = createDiffuseMaterial(Vec(0.2f, 0.9f, 0.2f));
	Material const* lightMat = createLightMaterial(Vec(10.f, 10.f, 10.f));

	GeometricPrimitive const* sphere = createSphere(Point(0.f, 0.f, 0.f), 1.f, greenSchlickMat);
	GeometricPrimitive const* wallDown = createPlane(Point(0.f, -4.f, 0.f), Vec(0.f, 1.f, 0.f), diffuseWallDownMat);
	GeometricPrimitive const* lightSource = createSphere(Point(0.f, 5.f, -1.f), 3.f, lightMat);

	std::vector<Primitive const*> scene;
	scene.push_back(sphere);
	scene.push_back(wallDown);
	scene.push_back(lightSource);
	
	Renderer renderer(params);
	Film film(XRES, YRES);
	renderer.render(cam, scene, film);

}

int main(int argc, char ** argv)
{
	sf::RenderWindow window(sf::VideoMode(XRES, YRES), 
							"SFML OpenGL", 
							sf::Style::Default, 
							sf::ContextSettings(32, 0, 8));
	texture.create(XRES, YRES);
	sf::RectangleShape drawRect(sf::Vector2f(XRES, YRES));
	drawRect.setTexture(&texture);
	rs.buffer = new uint32_t[XRES*YRES];
	std::stringstream progress_str;
	std::stringstream sps_str("??? SPS");
	sf::Clock clock;	// clock to measure SPS
	unsigned int lastSamples = 0;

	sf::Font font;
	if (!font.loadFromFile("uzura.ttf")) {
		std::cerr << "font.loadFromFile failed\n";
		return EXIT_FAILURE;
	}

	unsigned int numTexLines = 0;
	unsigned int curLine = 0;
	
	// launch render thread
	std::thread worker(&work);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
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

		// update texture
		curLine = rs.numRenderedLines;
		if (curLine > numTexLines) {
			/*texture.update((sf::Uint8*)(rs.buffer + XRES*numTexLines), XRES, 
							curLine-numTexLines, 0, numTexLines);*/
			texture.update((sf::Uint8*)(rs.buffer), XRES, YRES, 0, 0);
			numTexLines = curLine;
		}

		window.clear(sf::Color::Red);
		
		window.draw(drawRect);
		progress_str.str("");
		progress_str << curLine << " / " << YRES;
		sf::Text progress_text(progress_str.str(), font);
        progress_text.setColor(sf::Color(255, 255, 255, 255));
        progress_text.setPosition(10.f, 10.f);
        window.draw(progress_text);
        sf::Text sps_text(sps_str.str(), font);
        sps_text.setColor(sf::Color(255, 255, 255, 255));
        sps_text.setPosition(10.f, 38.f);
        window.draw(sps_text);
		
		window.display();

		long delta = clock.getElapsedTime().asMilliseconds();
		// update SPS display every 250ms
		if (delta >= 250) {
			sps_str.str("");
			sps_str << (rs.numSamples - lastSamples)*1000/delta;
			sps_str << " SPS";
			clock.restart();
			lastSamples = rs.numSamples;
		}
	}

	worker.join();

	std::stringstream file_name("output_");
	file_name << "output_" << time(NULL) << ".png";
	std::string str = file_name.str();

	std::clog << "\nSaving image to " << str.c_str() << "...";
	texture.copyToImage().saveToFile(str.c_str());
	std::clog << "Done\n";

	delete[] rs.buffer;

	return 0;
}


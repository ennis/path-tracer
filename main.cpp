#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <thread>
#include <atomic>
#include <sstream>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include "sphere.hpp"
#include "vec.hpp"
#include "ray.hpp"
#include "matrix4x4.hpp"
#include "camera.hpp"
#include "bbox.hpp"
#include "renderer.hpp"
#include "plane.hpp"

using namespace std;

static const unsigned int XRES = 800;
static const unsigned int YRES = 600;
static const float ASPECT_RATIO = static_cast<float>(XRES)/static_cast<float>(YRES);
	

sf::Texture texture;
RenderState rs;

template<typename T>
T clamp(T v, T min, T max)
{
	return (v < min) ? min : ((v > max) ? max : v);
}

void work()
{
	// setup camera
	rs.camera.eye = Point(0, 0, -10.0f);
	rs.camera.lookAt = Point(0.f, 0.f, 0.f);
	rs.camera.width = ASPECT_RATIO;
	rs.camera.height = 1.f;
	rs.camera.up = Vec(0.f, 1.f, 0.f);
	rs.camera.screenDist = 1.f;
	rs.camera.viewM = Matrix4x4::lookAtRH(rs.camera.eye, rs.camera.lookAt, rs.camera.up);
	// resolution
	rs.pixelWidth = XRES;
	rs.pixelHeight = YRES;
	// spp
	rs.samplesPerPixel = 10;
	rs.maxDepth = 7;
	rs.supersampling = true;
	
	// create sphere geom
	Sphere sphere(0.5f /* radius */ );
	Sphere light_sphere(2.f /* radius */);
	Sphere wall(9997.f);
	Plane wall_plane_geom(Vec(0, 0, -1.f));
	Plane wall_left_geom(Vec(1, 0, 0));
	Plane wall_right_geom(Vec(-1, 0, 0));

	// Diffuse mat
	Material walls_mat;
	walls_mat.reflectionType = R_DIFFUSE;
	walls_mat.brdfType = DIFFUSE_STD;
	walls_mat.specular = 0.0f;
	walls_mat.refractiveIndex = 1.0f;
	walls_mat.brdfData.R0 = 0.5f;

	// Specular mat
	Material spec_mat;
	spec_mat.reflectionType = R_SPECULAR;
	spec_mat.brdfType = DIFFUSE_SCHLICK;
	spec_mat.specular = 0.5f;
	spec_mat.refractiveIndex = 1.0f;
	walls_mat.brdfData.R0 = 0.5f;

	// Glass 
	Material glass;
	glass.reflectionType = R_REFRACTIVE;
	glass.brdfType = DUMMY;
	glass.specular = 0.0f;
	glass.refractiveIndex = 1.21f;

	// Mirror 
	Material mirror;
	mirror.reflectionType = R_REFLECTIVE;
	mirror.brdfType = DUMMY;
	mirror.specular = 0.0f;
	mirror.refractiveIndex = 1.21f;

	Object wall_down;
	wall_down.position = Point(0,-10000,0);
	wall_down.color = Vec(0.5,0.0,0.5);
	wall_down.emittance = Vec(0.0,0.0,0.0);
	wall_down.geometry = &wall;
	wall_down.material = &walls_mat;
	rs.scene.push_back(&wall_down);

	Object wall_up;
	wall_up.position = Point(0,10000,0);
	wall_up.color = Vec(0.5,0.2,0.0);
	wall_up.emittance = Vec(0.0,0.0,0.0);
	wall_up.geometry = &wall;
	wall_up.material = &walls_mat;
	rs.scene.push_back(&wall_up);

	Object wall_left;
	wall_left.position = Point(-6,0,0);
	wall_left.color =  Vec(1.0,0.8,0.0);
	wall_left.emittance = Vec(0.0,0.0,0.0);
	wall_left.geometry = &wall_left_geom;
	wall_left.material = &mirror;
	rs.scene.push_back(&wall_left);

	Object wall_right;
	wall_right.position = Point(6,0,0);
	wall_right.color = Vec(0.0,0.5,0.2);
	wall_right.emittance = Vec(0.0,0.0,0.0);
	wall_right.geometry = &wall_right_geom;
	wall_right.material = &mirror;
	rs.scene.push_back(&wall_right);

	Object wall_back;
	wall_back.position = Point(0,0,-10000);
	wall_back.color = Vec(0.2,0.2,0.2);
	wall_back.emittance = Vec(0.0,0.0,0.0);
	wall_back.geometry = &wall;
	wall_back.material = &walls_mat;
	//rs.scene.push_back(&wall_back);

	Object small_sphere;
	small_sphere.position = Point(4,0,0);
	small_sphere.color = Vec(1.0,1.0,1.0);
	small_sphere.emittance = Vec(0.0,0.0,0.0);
	small_sphere.geometry = &sphere;
	small_sphere.material = &mirror;
	rs.scene.push_back(&small_sphere);

	Object glass_sphere;
	glass_sphere.position = Point(-2,-2,0);
	glass_sphere.color = Vec(1.0,1.0,1.0);
	glass_sphere.emittance = Vec(0.0,0.0,0.0);
	glass_sphere.geometry = &sphere;
	glass_sphere.material = &glass;
	rs.scene.push_back(&glass_sphere);

	Object light_source;
	light_source.position = Point(0,0,0);
	light_source.color = Vec(1.0,1.0,1.0);
	light_source.emittance = Vec(4.0,4.0,4.0);
	light_source.geometry = &light_sphere;
	light_source.material = NULL;
	rs.scene.push_back(&light_source);

	Object back_plane;
	back_plane.position = Point(0,0,5.0);
	back_plane.color = Vec(1.0,0.2,1.0);
	back_plane.emittance = Vec(0.0,0.0,0.0);
	back_plane.geometry = &wall_plane_geom;
	back_plane.material = &walls_mat;
	rs.scene.push_back(&back_plane);

	render(rs);
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
			texture.update((sf::Uint8*)(rs.buffer + XRES*numTexLines), XRES, 
							curLine-numTexLines, 0, numTexLines);
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

	std::clog << "\nSaving image to output.png...";
	texture.copyToImage().saveToFile("output.png");
	std::clog << "Done\n";

	delete[] rs.buffer;

	return 0;
}


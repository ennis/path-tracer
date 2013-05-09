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
	rs.samplesPerPixel = 50;
	rs.maxDepth = 5;
	rs.supersampling = true;
	rs.cosineWeightedSampling = false;
	rs.directLightingOnly = true;
	rs.explicitLightSampling = true;
	
	// create sphere geom
	Sphere sphere(1.1f /* radius */ );
	Sphere light_sphere(0.7f /* radius */);
	Sphere sphere1geom(1.05f /* radius */ );
	Sphere sphere2geom(1.1f /* radius */);

	Plane wall_up_geom(Vec(0, -1, 0));	// facing down
	Plane wall_down_geom(Vec(0, 1, 0));		// facing up

	Plane wall_plane_geom(Vec(0, 0, -1.f));
	Plane wall_left_geom(Vec(1, 0, 0));
	Plane wall_right_geom(Vec(-1, 0, 0));

	Triangle triangle_geom(Point(4.0f, 3.5f, 1.f),
							Point(1.0f, 1.0f, 1.5f),
							Point(4.0f, 1.0f, 1.5f));

	GlassBSDF glass_bsdf(1.5f);
	GlassBSDF transparent_glass_bsdf(1.5f, 0.f);
	DiffuseBSDF diffuse_bsdf;
	MirrorBSDF mirror_bsdf;
	SpecularBSDF specular_bsdf(200.f);
	SchlickBSDF schlick_bsdf(0.08f);
	PhongBSDF phong_bsdf(50);
	AshikhminShirleyBSDF ash_bsdf(1.0f, 1.0f, 10.f, 100.f);

	//Object obj(new Sphere(), Color(), new GlassMaterial(), )

	// Diffuse mat
	/*Material walls_mat;
	walls_mat.reflectionType = R_DIFFUSE;
	walls_mat.brdfType = DIFFUSE_STD;
	walls_mat.specular = 0.0f;
	walls_mat.refractiveIndex = 1.0f;
	walls_mat.brdfData.R0 = 0.5f;*/

	// Specular mat
	/*Material spec_mat;
	spec_mat.reflectionType = R_SPECULAR;
	spec_mat.brdfType = DIFFUSE_SCHLICK;
	spec_mat.specular = 0.5f;
	spec_mat.refractiveIndex = 1.0f;
	walls_mat.brdfData.R0 = 0.5f;*/

	// Glass 
	/*Material glass;
	glass.reflectionType = R_REFRACTIVE;
	glass.brdfType = DUMMY;
	glass.specular = 0.0f;
	glass.refractiveIndex = 1.21f;*/

	// Mirror 
	/*Material mirror;
	mirror.reflectionType = R_REFLECTIVE;
	mirror.brdfType = DUMMY;
	mirror.specular = 0.0f;
	mirror.refractiveIndex = 1.21f;*/

	// TODO MultiBSDF

	Object wall_down(
		/* position */ Point(0,-4,0),
		/* color */ Vec(0.5,0.0,0.5),
		/* emittance */ Vec(0.0,0.0,0.0),
		/* geometry */ &wall_down_geom,
		/* bsdf */ &diffuse_bsdf);
	rs.scene.push_back(&wall_down);

	Object wall_up(
		/* position */ Point(0,4,0),
		/* color */ Vec(0.5,0.2,0.0),
		/* emittance */ Vec(0.0,0.0,0.0),
		/* geometry */ &wall_up_geom,
		/* bsdf */ &diffuse_bsdf);
	rs.scene.push_back(&wall_up);

	Object wall_left(
		/* position */ Point(-6,0,0),
		/* color */ Vec(1.0,0.8,0.0),
		/* emittance */ Vec(0.0,0.0,0.0),
		/* geometry */ &wall_left_geom,
		/* bsdf */ &diffuse_bsdf);
	rs.scene.push_back(&wall_left);

	Object wall_right(
		/* position */ Point(6,0,0),
		/* color */ Vec(0.0,0.5,0.2),
		/* emittance */ Vec(0.0,0.0,0.0),
		/* geometry */ &wall_right_geom,
		/* bsdf */ &diffuse_bsdf);
	rs.scene.push_back(&wall_right);

	Object wall_back(
		/* position */ Point(0,0,5),
		/* color */ Vec(1.0,1.0,1.0),
		/* emittance */ Vec(0.0,0.0,0.0),
		/* geometry */ &wall_plane_geom,
		/* bsdf */ &diffuse_bsdf);
	rs.scene.push_back(&wall_back);

	Object light1(
		/* position */ Point(0,2.5f,1),
		/* color */ Vec(1.0,0.2,1.0),
		/* emittance */ Vec(10.0,10.0,5.0),
		/* geometry */ &light_sphere,
		/* bsdf */ &diffuse_bsdf);
	rs.scene.push_back(&light1);

	Object light2(
		/* position */ Point(-3,2.5f,1),
		/* color */ Vec(1.0,0.2,1.0),
		/* emittance */ Vec(10.0,10.0,5.0),
		/* geometry */ &light_sphere,
		/* bsdf */ &diffuse_bsdf);
	rs.scene.push_back(&light2);

	
	Object light3(
		/* position */ Point(3,2.5f,1),
		/* color */ Vec(1.0,0.2,1.0),
		/* emittance */ Vec(10.0,10.0,5.0),
		/* geometry */ &light_sphere,
		/* bsdf */ &diffuse_bsdf);
	rs.scene.push_back(&light3);

	/* triangle test */
	Object triangle(
		/* position */ Point(0,0,0),
		/* color */ Vec(0.0,0.2,1.0),
		/* emittance */ Vec(0.0,0.0,0.0),
		/* geometry */ &triangle_geom,
		/* bsdf */ &mirror_bsdf);
	//rs.scene.push_back(&triangle);

	Object small_sphere(
		/* position */ Point(3.f,2.5f,1.f),
		/* color */ Vec(1.0,1.0,1.0),
		/* emittance */ Vec(0.0,0.0,0.0),
		/* geometry */ &sphere,
		/* bsdf */ &phong_bsdf);
	//rs.scene.push_back(&small_sphere);

	Object glass_sphere(
		/* position */ Point(0,-1.5f,0),
		/* color */ Vec(1,1,1),
		/* emittance */ Vec(0.0,0.0,0.0),
		/* geometry */ &sphere,
		/* bsdf */ &glass_bsdf);
	//rs.scene.push_back(&glass_sphere);

	Object sphere1(
		/* position */ Point(-3,-1.5f,0),
		/* color */ Vec(0,1,0),
		/* emittance */ Vec(0.0,0.0,0.0),
		/* geometry */ &sphere1geom,
		/* bsdf */ &schlick_bsdf);
	rs.scene.push_back(&sphere1);

	Object sphere2(
		/* position */ Point(-3,-1.5f,0),
		/* color */ Vec(1,1,1),
		/* emittance */ Vec(0.0,0.0,0.0),
		/* geometry */ &sphere2geom,
		/* bsdf */ &glass_bsdf);
	//rs.scene.push_back(&sphere2);

	Object glass2_sphere(
		/* position */ Point(3,-1.5f,0),
		/* color */ Vec(1,1,1),
		/* emittance */ Vec(0.0,0.0,0.0),
		/* geometry */ &sphere,
		/* bsdf */ &transparent_glass_bsdf);
	//rs.scene.push_back(&glass2_sphere);

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


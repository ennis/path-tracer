#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <memory>
#include "size.hpp"
#include "boundingbox.hpp"
#include <cstdint>

namespace ui
{

typedef int TextureID;
typedef int ShaderID;

struct Color
{
	Color() : r(0.f), g(0.f), b(0.f), a(0.f)
	{}

	Color(float r_, float g_, float b_, float a_) : r(r_), g(g_), b(b_), a(a_)
	{}

	uint32_t toUInt32ARGB() const {
		//uint8_t ab, rb, gb, bb;
		return 0;	// TODO
	}

	float r, g, b, a;
};

class Renderer
{
	typedef std::shared_ptr<Renderer> Ptr;
public:
	virtual ShaderID loadShaderFromSource(char const *source) = 0;
	virtual ShaderID loadShaderFromFile(char const *file) = 0;
	virtual TextureID loadTextureFromFile(char const *file, int &width, int &height) = 0;
	
	virtual void beginDraw() = 0;
	virtual void endDraw() = 0;

	virtual void drawRect(BoundingBox const &bb, Color const &color) = 0;
	virtual void drawTexturedRect(BoundingBox const &bb, TextureID texID) = 0;
	virtual void useShader(ShaderID shaderID) = 0;
	
	// XXX shader programs?

protected:
};

}

#endif
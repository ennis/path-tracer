#ifndef BOUNDINGBOX_HPP
#define BOUNDINGBOX_HPP

#include <ostream>
#include <algorithm>
#include "margins.hpp"

namespace ui
{

struct BoundingBox
{
	BoundingBox() : 
	x(0), y(0), width(0), height(0)
	{}

	BoundingBox(int x_, int y_, int width_, int height_) : 
	x(x_), y(y_), width(width_), height(height_)
	{}

	BoundingBox contract(Margins const &margins) {
		return BoundingBox(x + margins.left, y + margins.top, 
				std::max(0, width - margins.left - margins.right), 
				std::max(0, height - margins.top - margins.bottom));
	}

	bool hitTest(int tx, int ty) {
		if ((x < tx) && 
			(tx < (x + width)) && 
			(y < ty) && 
			(ty < (y + height))) { 
			return true;
		} else {
			return false;
		} 
	}

	int x;
	int y;
	int width;
	int height;
};

}

static inline std::ostream &operator<<(std::ostream &os, ui::BoundingBox const &bb)
{
	os << "(" << bb.x << "," << bb.y << ":" <<bb.width << "x" << bb.height << ")";
	return os;
}

#endif
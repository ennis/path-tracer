#ifndef SIZE_HPP
#define SIZE_HPP

#include <iostream>
#include "margins.hpp"

namespace ui
{

struct Size
{
	static const int ADJUST = -2;
	static const int FILL = -1;
	
	Size() : width(FILL), height(FILL) 
	{}

	Size(int width_, int height_) : width(width_), height(height_)
	{}

	bool hasWidth() const {
		return width >= 0;
	}

	bool hasHeight() const {
		return height >= 0;
	}

	Size &expand(Margins const &margins) {
		if (hasWidth()) {
			width += margins.left + margins.right;
		}
		if (hasHeight()) {
			height += margins.top + margins.bottom;
		}
		return *this;
	}
	
	int width;
	int height;
};

}

static void printSizeComponent(std::ostream &os, int value)
{
	switch (value) {
		case -2:
			os << "Adjust";
			break;
		case -1:
			os << "Fill";
			break;
		default:
			os << value;
			break;
	}
}

static std::ostream &operator<<(std::ostream &os, ui::Size const &size) {
	os << "width:";
	printSizeComponent(os, size.width);
	os << " height:";
	printSizeComponent(os, size.height);
}

#endif
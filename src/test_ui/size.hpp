#ifndef SIZE_HPP
#define SIZE_HPP

#include <iostream>
#include "margins.hpp"

namespace ui
{

struct Size
{
	Size() : width(-1), height(-1) 
	{}

	Size(int width_, int height_) : width(width_), height(height_)
	{}

	bool hasWidth() const {
		return width != -1;
	}

	bool hasHeight() const {
		return height != -1;
	}

	Size &expand(Margins const &margins) {
		if (hasWidth()) {
			width += margins.left + margins.right;
		}
		if (hasHeight()) {
			height += margins.top + margins.bottom;
		}
	}
	
	int width;
	int height;
};

}

static std::ostream &operator<<(std::ostream &os, ui::Size const &size) {
	if (size.hasWidth()) {
		os << size.width;
	} else {
		os << "<auto>";
	}
	os << 'x';
	if (size.hasHeight()) {
		os << size.height;
	} else {
		os << "<auto>";
	}
}

#endif
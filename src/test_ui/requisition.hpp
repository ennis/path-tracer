#ifndef REQUISITION_HPP
#define REQUISITION_HPP

#include "margins.hpp"

namespace ui
{

struct Requisition
{
	Requisition() : width(-1), height(-1)
	{}

	Requisition(int width_, int height_) : width(width_), height(height_)
	{}

	void applyMargins(Margins const &margins)
	{
		width += margins.left + margins.right;
		height += margins.top + margins.bottom;
	}

	int width, height;
};

}

#endif
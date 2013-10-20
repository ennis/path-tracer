#ifndef BOXELEMENT_HPP
#define BOXELEMENT_HPP

#include "element.hpp"
#include "boundingbox.hpp"
#include "size.hpp"

namespace ui
{

class Box
{
public:
	Box();

	void setWidth(Size width);
	void setHeight(Size height);

	void calculateRequisition(int containerWidth, int containerHeight, int contentWidth, int contentHeight, int &reqWidth, int &reqHeight);
	void calculateBounds(BoundingBox const &allocation, BoundingBox &bounds, BoundingBox &content);

protected:

	int calculateMargins();

};

}

#endif
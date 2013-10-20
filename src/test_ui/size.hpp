#ifndef SIZE_HPP
#define SIZE_HPP

#include <iostream>

namespace ui
{

struct Size
{
	enum SizeType {
		PERCENTAGE,
		PIXELS,
		AUTO
	};

	SizeType type;
	int value;		// -1 is auto or 100%

	Size() : value(-1), type(AUTO)
	{}

	Size(int v, SizeType t = PIXELS) : value(v), type(t)
	{}

	static Size percentage(int percent) {
		Size ret;
		ret.type = PERCENTAGE;
		ret.value = percent;
		return ret;
	}

	static Size px(int pixels) {
		Size ret;
		ret.type = PIXELS;
		ret.value = pixels;
		return ret;
	}

	int toPixels(int containingBlockSize) {
		if (type == PERCENTAGE) {
			return containingBlockSize * value / 100;
		} else if (type == PIXELS) {
			return value;
		} else {
			return containingBlockSize;
		}
	}

	void dump(std::ostream &os) const {
		if (value == -1) {
			os << "auto";
		} else {
			os << value << (type == PIXELS) ? "px" : "%";
		}
	}

	bool operator==(Size const &rhs) const {
		return value == rhs.value && type == rhs.type;
	}
};

}
#endif
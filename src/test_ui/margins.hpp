#ifndef MARGINS_HPP
#define MARGINS_HPP


namespace ui
{

struct Margins
{
	Margins() : left(0), right(0), top(0), bottom(0)
	{}

	Margins(int left_, int right_, int top_, int bottom_) : left(left_), right(right_), top(top_), bottom(bottom_)
	{}

	void add(Margins const &m) {
		left += m.left;
		right += m.right;
		top += m.top;
		bottom += m.bottom;
	}

	int left, right, top, bottom;
};

}


#endif
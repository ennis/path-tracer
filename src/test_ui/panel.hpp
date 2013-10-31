#ifndef VBOX_HPP
#define VBOX_HPP

#include "container.hpp"

#include <memory>

namespace ui
{

class Panel : public Container
{
	struct private_key{};
public:
	typedef std::shared_ptr<Panel> Ptr;

	enum Orientation {
		VERTICAL,
		HORIZONTAL
	};

	virtual ~Panel();

	virtual void render(sf::RenderTarget &renderTarget);
	virtual void setOrientation(Orientation orientation);
	
	Panel(private_key);
	Panel(private_key, Orientation orientation);

	virtual Size getDesiredSize();
	virtual void doLayout();

	static Ptr create() {
		return std::make_shared<Panel>(private_key());
	}

	static Ptr create(Orientation orientation) {
		return std::make_shared<Panel>(private_key(), orientation);
	}

private:
	Orientation m_orientation;
};


}

#endif
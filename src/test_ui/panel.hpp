#ifndef PANEL_HPP
#define PANEL_HPP

#include "container.hpp"

#include <memory>

namespace ui
{

class Panel : public Container
{
	struct private_key{};
public:
	typedef std::shared_ptr<Panel> Ptr;

	enum Spacing {
		SPACING_EVEN,
		NO_SPACING
	};

	enum Orientation {
		VERTICAL,
		HORIZONTAL
	};

	virtual ~Panel();

	virtual void render(sf::RenderTarget &renderTarget, Engine &engine);
	virtual void setOrientation(Orientation orientation);
	
	Panel(private_key);
	Panel(private_key, Orientation orientation, Spacing spacing);

	virtual Size getDesiredSize(Engine &engine);
	virtual void doLayout(Engine &engine);

	static Ptr create() {
		return std::make_shared<Panel>(private_key());
	}

	static Ptr create(Orientation orientation, Spacing spacing) {
		return std::make_shared<Panel>(private_key(), orientation, spacing);
	}

private:
	Orientation m_orientation;
	Spacing m_spacing;
};


}

#endif
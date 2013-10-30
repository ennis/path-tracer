#ifndef SPACER_HPP
#define SPACER_HPP

#include "element.hpp"

namespace ui
{

class Spacer : public Element
{
	struct private_key{};
public:
	typedef std::shared_ptr<Spacer> Ptr;
	
	Spacer(private_key)
	{
		setSize(Size(Size::FILL, Size::FILL));
	}
	
	virtual ~Spacer()
	{}
	
	virtual void render(sf::RenderTarget& renderTarget)
	{
		// nothing to do
	}
	
	static Ptr create() {
		return std::make_shared<Spacer>(private_key());
	}
	
private:
};

}

#endif
#ifndef TEXTBOX_HPP
#define TEXTBOX_HPP

#include "element.hpp"

#include <string>

namespace ui
{

class TextBox : public Element
{
	struct private_key{};
public:
	typedef std::shared_ptr<TextBox> Ptr;

	virtual ~TextBox();
	virtual void render(sf::RenderTarget &renderTarget);
	
	virtual Size getDesiredSize();
	
	TextBox(private_key, std::string const &str) : 
		m_string(str)
	{}

	std::string const &getText() const {
		return m_string;
	}

	// TODO move semantics
	void setText(std::string const &str) {
		m_string = str;
	}

	static Ptr create(std::string const &str)
	{
		return std::make_shared<TextBox>(private_key(), str);
	}

private:

	std::string m_string;
};

}

#endif

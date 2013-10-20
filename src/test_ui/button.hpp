#ifndef BUTTON_HPP
#define BUTTON_HPP

#include "ui.hpp"
#include "element.hpp"
#include "textbox.hpp"

#include <string>

namespace ui
{
	class Button : public Element
	{
		struct private_key{};
	public:
		typedef std::shared_ptr<Button> Ptr;

		~Button()
		{}

		virtual void render(sf::RenderTarget &renderTarget, Engine &engine);
		virtual void onSetAllocation();
		
		Button(private_key, std::string const &label);

		static Button::Ptr create(std::string const &label) {
			return std::make_shared<Button>(private_key(), label);
		}

	private:

		TextBox::Ptr m_label;
	};
}

#endif
#ifndef CHECKBOX_HPP
#define CHECKBOX_HPP

#include "element.hpp"

namespace ui
{

class CheckBox : public Element
{
	struct private_key {};
public:
	
	typedef std::shared_ptr<CheckBox> Ptr;
	
	CheckBox(private_key, bool checked = false) : m_checked(checked)
	{}
	
	virtual ~CheckBox()
	{}
	
	virtual Size getDesiredSize();
	virtual void doLayout();
	virtual void render(sf::RenderTarget& renderTarget);
	
	bool isChecked() const {
		return m_checked;
	}
	
	void setChecked(bool newCheck) {
		m_checked = newCheck;
	}
	
	void onClick(int mouseX, int mouseY);
	
	static Ptr create(bool checked = false) {
		return std::make_shared<CheckBox>(private_key(), checked);
	}
	
private:
	bool m_checked;
};
	
}

#endif
#ifndef RADIOBUTTON_HPP
#define RADIOBUTTON_HPP

#include "element.hpp"
#include <vector>

namespace ui
{
	
class RadioGroup;
class RadioButton;

class RadioGroup
{
	struct private_key {};
public:
	typedef std::shared_ptr<RadioGroup> Ptr;
	RadioGroup(private_key)
	{}
	
	void addRadio(std::weak_ptr<RadioButton> radio) {
		m_radios.push_back(radio);
	}
	
	void removeRadio(RadioButton &radio);
	
	void radioClicked(RadioButton &radioButton);
	
	static Ptr create() {
		return std::make_shared<RadioGroup>(private_key());
	}
	
private:
	std::vector<std::weak_ptr<RadioButton> > m_radios;
};

class RadioButton : public Element
{
	struct private_key {};
public:
	typedef std::shared_ptr<RadioButton> Ptr;
	RadioButton(private_key) : m_checked(false)
	{}
	
	virtual ~RadioButton() {
		if (m_radioGroup) {
			m_radioGroup->removeRadio(*this);
		}
	}
	
	virtual void render(sf::RenderTarget& renderTarget);
	virtual void doLayout();
	virtual Size getDesiredSize();
	
	bool isChecked() const {
		return m_checked;
	}
	
	void setChecked(bool newCheck) {
		forceChecked(newCheck);
		if (m_radioGroup) {
			m_radioGroup->radioClicked(*this);
		}
	}
	
	void forceChecked(bool newCheck) {
		m_checked = newCheck;
	}
	
	void setRadioGroup(RadioGroup::Ptr radioGroup) {
		m_radioGroup = radioGroup;
		radioGroup->addRadio(std::weak_ptr<RadioButton>(std::static_pointer_cast<RadioButton>(shared_from_this())));	// XXX meh.
	}
	
	void onClick(int mouseX, int mouseY) {
		setChecked(true);
	}
	
	static Ptr create() {
		return std::make_shared<RadioButton>(private_key());
	}
	
protected:
	RadioGroup::Ptr m_radioGroup;
	bool m_checked;
};

/*class RadioOptions : public Container
{
	struct private_key {};
public:
	typedef std::shared_ptr<RadioOptions> Ptr;

	RadioOptions(private_key) : 
	{}
	
	virtual ~RadioButton() {
	}
	
	virtual void render(sf::RenderTarget& renderTarget) {
		
	}
	
	virtual void doLayout();
	virtual Size getDesiredSize();
	
protected:
	RadioGroup::Ptr m_radioGroup;
	std::vector<TextBox::Ptr> m_labels;
};*/

}

#endif
#include "radiobutton.hpp"

namespace ui
{

void RadioGroup::radioClicked(RadioButton &radioButton)
{
	for (auto radio : m_radios) {
		RadioButton::Ptr ptr = radio.lock();
		if (ptr && ptr.get() != &radioButton) {
			ptr->forceChecked(false);
		}
	}
}

void RadioGroup::removeRadio(RadioButton &radioButton)
{
	std::remove_if(m_radios.begin(), m_radios.end(), [&](std::weak_ptr<RadioButton> e) {
		RadioButton::Ptr ptr = e.lock();
		return (ptr && ptr.get() == &radioButton);
	});
}

	

void RadioButton::render(sf::RenderTarget& renderTarget, Engine& engine)
{
	engine.drawRadioButton(renderTarget, *this);
}

void RadioButton::doLayout(Engine& engine)
{
	// XXX nothing to do
}

Size RadioButton::getDesiredSize(Engine& engine)
{
	return engine.getRadioSize();
}

	
}
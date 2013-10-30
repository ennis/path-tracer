#ifndef SLIDER_HPP
#define SLIDER_HPP

#include "element.hpp"

namespace ui
{

class Slider : public Element
{
	struct private_key{};
public:
	
	typedef std::shared_ptr<Slider> Ptr;

	Slider(private_key, int minValue, int maxValue, int initialValue) 
	{
		// TODO corner case?
		if (minValue > maxValue) {
			m_sliderMinValue = 0;
			m_sliderMaxValue = 0;
			m_sliderValue = 0;
		} else {
			m_sliderMinValue = minValue;
			m_sliderMaxValue = maxValue;
			// XXX can't give a shared_ptr reference to an object not fully constructed (by make_shared)
			//setValue(initialValue);
			m_sliderValue = std::max(std::min(initialValue, maxValue), minValue);
		}
	}

	virtual ~Slider()
	{}
	
	virtual void render(sf::RenderTarget &renderTarget);
	virtual Size getDesiredSize();
	virtual void doLayout();
	//virtual void onDrag(int mouseX, int mouseY);
	virtual void onClick(int mouseX, int mouseY);
	
	void setMinValue(int minValue) {
		m_sliderMinValue = minValue;
		m_sliderValue = std::max(m_sliderValue, minValue);
	}
	
	int getMinValue() const {
		return m_sliderMinValue;
	}

	void setMaxValue(int maxValue) {
		m_sliderMaxValue = maxValue;
		m_sliderValue = std::min(m_sliderValue, maxValue);
	}

	int getMaxValue() const {
		return m_sliderMaxValue;
	}

	void setValue(int value) {
		m_sliderValue = std::max(std::min(value, m_sliderMaxValue), m_sliderMinValue);
		onSliderValueChanged();
	}

	int getValue() const {
		return m_sliderValue;
	}

	static Ptr create(int minValue, int maxValue, int initialValue) {
		return std::make_shared<Slider>(private_key(), minValue, maxValue, initialValue);
	}

	// events
	typedef Delegate1<Slider::Ptr> OnSliderValueChanged;
	
	virtual void onSliderValueChanged() {
		onSliderValueChangedEvent(std::static_pointer_cast<Slider>(shared_from_this()));
	}

	virtual void onHover(int mouseX, int mouseY) {
		if (testState(Element::ACTIVE)) {
			setValue(getEngine().getSliderPosition(*this, mouseX, mouseY));
		}
		Element::onHover(mouseX, mouseY);
	}

public:
	OnSliderValueChanged onSliderValueChangedEvent;

private:
	int m_sliderValue;
	int m_sliderMinValue;
	int m_sliderMaxValue;
};

}

#endif
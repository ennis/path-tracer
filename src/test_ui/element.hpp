#ifndef UI_ELEMENT_HPP
#define UI_ELEMENT_HPP

#include "ui.hpp"
#include "delegate.hpp"
#include "engine.hpp"
#include "boundingbox.hpp"
#include "size.hpp"
#include "margins.hpp"
#include "requisition.hpp"
#include "sizepolicy.hpp"

#include <SFML/Graphics.hpp>

#include <memory>
#include <vector>


namespace ui
{

class Engine;

class Element : public std::enable_shared_from_this<Element>
{
public:
	// Element::Ptr
	typedef std::shared_ptr<Element> Ptr;
	typedef std::shared_ptr<Engine> EnginePtr;
	
	//=========================
	// States
	enum StateBits {
		VISIBLE = 1 << 0,
		HOVER = 1 << 1,
		ACTIVE = 1 << 2,
		FOCUS = 1 << 3
	};

	enum Placement {
		TL,
		TC,
		TR,
		CL,
		CC,
		CR,
		BL,
		BC,
		BR
	};

	//=========================
	// Default constructor
	Element() : 
		m_class(""), 
		m_id(""),
		m_state(VISIBLE),
		m_requisition(),
		m_bounds(BoundingBox()),
		m_contents(BoundingBox()),
		m_padding(),
		m_margin(),
		m_parent()
	{}

	Element(std::string const &className, std::string const &elementId) :  m_state(VISIBLE), m_class(className), m_id(elementId)
	{}

	virtual void render(sf::RenderTarget &renderTarget, Engine &engine) = 0;		// XXX this is really an instance of the observer pattern

	std::string const &getClass() {
		return m_class;
	}
	std::string const &getId() {
		return m_id;
	}

	//==================================
	// layout
	void layout(Engine &engine, BoundingBox const &bounds) {
		m_bounds = bounds;
		doLayout(engine);
	}

	virtual void doLayout(Engine &engine)
	{}
	
	virtual Size getDesiredSize(Engine &engine) {
		return m_size;
	}
	
	Size measureFromContents(Size const &contents) {
		Size result;
		if (m_size.width == Size::ADJUST) {
			result.width = contents.width;
		} else if (m_size.width == Size::FILL) {
			result.width = Size::FILL;
		} else {
			result.width = m_size.width;
		}
		
		if (m_size.height == Size::ADJUST) {
			result.height = contents.height;
		} else if (m_size.height == Size::FILL) {
			result.height = Size::FILL;
		} else {
			result.height = m_size.height;
		}
		
		return result;		
	}

	
	//==================================
	// fixed size

	Size getSize() const {
		return m_size;
	}

	void setSize(Size newSize) {
		m_size = newSize;
	}

	//==================================
	// additional margins and padding
	void setMargin(Margins const &margin) {
		m_margin = margin;
	}

	void setPadding(Margins const &padding) {
		m_padding = padding;
	}

	void setParent(Element::Ptr parent) {
		std::clog << "setParent " << this << "->" << parent << "\n";
		m_parent = parent;
	}

	void setBounds(BoundingBox const &bb) {
		m_bounds = bb;
	}

	BoundingBox const &getBounds() const {
		return m_bounds;
	}

	/*BoundingBox const &getAllocation() const {
		return m_allocation;
	}*/

	/*void setPlacement(Placement placement) {
		m_placement = placement;
	}*/

	virtual bool hitTest(int x, int y) const;

	//============================
	// State query and manipulation
	bool testState(int stateBit) const;
	void setState(int stateBit);
	void clearState(int stateBit);
	
	//============================
	// SFML event handler
	virtual bool handleEvent(sf::Event const &event);

	// Event typedefs
	typedef Delegate3<Element::Ptr, int, int> OnClickEvent;
	typedef Delegate1<Element::Ptr> OnHoverEvent;
	typedef Delegate2<Element::Ptr, sf::Event::KeyEvent const &> OnKeyEvent;
	typedef Delegate3<Element::Ptr, int, int> OnDragEvent;
	
	virtual void onHoverEnter();
	virtual void onHoverLeave();
	virtual void onClick(int mouseX, int mouseY);
	virtual void onKeyDown(sf::Event::KeyEvent const &keyEvent);
	virtual void onKeyUp(sf::Event::KeyEvent const &keyEvent);
	virtual void onDrag(int mouseX, int mouseY);

public:
	// events
	OnClickEvent onClickEvent;
	OnHoverEvent onHoverEnterEvent;
	OnHoverEvent onHoverLeaveEvent;
	OnKeyEvent onKeyDownEvent;
	OnKeyEvent onKeyUpEvent;
	OnDragEvent onDragEvent;

protected:

	// available space -> requisition (-> requisition) -> placement -> allocation (-> recurse)
	

	// class name for properties
	std::string m_class;

	// element id
	std::string m_id;

	// State
	int m_state;

	// requisitions
	Requisition m_requisition;

	// element bounds (== hitbox)
	BoundingBox m_bounds;
	// content box
	BoundingBox m_contents;

	Size m_size;

	// additional margins
	Margins m_padding;
	Margins m_margin;

	std::weak_ptr<Element> m_parent;

};

}

#endif
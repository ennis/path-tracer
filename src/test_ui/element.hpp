#ifndef UI_ELEMENT_HPP
#define UI_ELEMENT_HPP

#include "ui.hpp"
#include "delegate.hpp"
#include "engine.hpp"
#include "boundingbox.hpp"
#include "size.hpp"

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
		m_reqWidth(-1),
		m_reqHeight(-1),
		m_allocation(BoundingBox()),
		m_bounds(BoundingBox()),
		m_boxWidth(Size()),
		m_boxHeight(Size()),
		m_marginBoxWidth(0),
		m_marginBoxHeight(0),
		m_paddingLeft(2),
		m_paddingRight(2),
		m_paddingTop(2),
		m_paddingBottom(2),
		m_marginLeft(2),
		m_marginRight(2),
		m_marginTop(2),
		m_marginBottom(2),
		m_placement(TL),
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

	virtual void setAllocation(BoundingBox const &newBB);
	virtual void calculateRequisition();
	virtual void doPlacement();
	virtual void getRequisition(int &reqWidth, int &reqHeight);
	virtual void setWidth(Size const &width);
	virtual void setHeight(Size const &height);

	void setParent(Element::Ptr parent) {
		std::clog << "setParent " << this << "->" << parent << "\n";
		m_parent = parent;
	}
	
	void layoutUpdated();

	BoundingBox const &getBounds() const {
		return m_bounds;
	}

	BoundingBox const &getAllocation() const {
		return m_allocation;
	}

	void setPlacement(Placement placement) {
		m_placement = placement;
	}

	// called after setAllocation
	virtual void onSetAllocation();

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

	virtual void onHoverEnter();
	virtual void onHoverLeave();
	virtual void onClick(int mouseX, int mouseY);
	virtual void onKeyDown(sf::Event::KeyEvent const &keyEvent);
	virtual void onKeyUp(sf::Event::KeyEvent const &keyEvent);

public:
	// events
	OnClickEvent onClickEvent;
	OnHoverEvent onHoverEnterEvent;
	OnHoverEvent onHoverLeaveEvent;
	OnKeyEvent onKeyDownEvent;
	OnKeyEvent onKeyUpEvent;

protected:

	// available space -> requisition (-> requisition) -> placement -> allocation (-> recurse)
	

	// class name for properties
	std::string m_class;

	// element id
	std::string m_id;

	// State
	int m_state;

	// requisitions
	int m_reqWidth, m_reqHeight;

	// Allocated space 
	BoundingBox m_allocation;

	// element bounds (== hitbox)
	BoundingBox m_bounds;

	// box size
	Size m_boxWidth;
	Size m_boxHeight;

	// calculated margins
	int m_marginBoxWidth;
	int m_marginBoxHeight;
	int m_paddingLeft;
	int m_paddingRight;
	int m_paddingTop;
	int m_paddingBottom;
	int m_marginLeft;
	int m_marginRight;
	int m_marginTop;
	int m_marginBottom;

	// TODO fixed positions

	// placement
	Placement m_placement;

	std::weak_ptr<Element> m_parent;

};

}

#endif
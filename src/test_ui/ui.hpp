#ifndef UI_HPP
#define UI_HPP

#include <memory>
#include <SFML/Graphics.hpp> 
#include "renderer.hpp"
#include "engine.hpp"
#include "defaultengine.hpp"

namespace ui
{

class Element;

class UI 
{
	typedef std::shared_ptr<Element> ElementPtr;
	struct private_key {};
public:

	typedef std::shared_ptr<UI> Ptr;

	void setEngine(Engine::Ptr engine);
	void render(sf::RenderTarget &renderTarget);
	void add(ElementPtr element);
	void setSize(int width, int height);
	void onEvent(sf::Event const &event);
	void doLayout();

	static UI::Ptr create(Engine::Ptr engine)
	{
		return std::make_shared<UI>(private_key(), engine);
	}

	UI(private_key, Engine::Ptr engine) : m_engine(engine)
	{
	}

private:

	int m_width;
	int m_height;
	//RenderList m_renderList;
	ElementPtr m_rootPanel;
	Engine::Ptr m_engine;
};

}

#endif
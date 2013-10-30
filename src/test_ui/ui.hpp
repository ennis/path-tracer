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

	void render(sf::RenderTarget &renderTarget);
	void add(ElementPtr element);
	void setSize(int width, int height);

	void onEvent(sf::Event const &event);
	void doLayout();

	static UI::Ptr create()
	{
		return std::make_shared<UI>(private_key());
	}

	UI(private_key) 
	{
	}

	static void setEngine(Engine::Ptr const &engine) {
		sEngine = engine;
	}

	static Engine &getEngine() {
		if (sEngine) {
			return *sEngine.get();
		} else {
			// XXX do something? create on first use?
			throw new std::exception("UI: no engine set");
		}
	}

private:

	int m_width;
	int m_height;
	//RenderList m_renderList;
	ElementPtr m_rootPanel;
	
	static Engine::Ptr sEngine;
};

}

#endif
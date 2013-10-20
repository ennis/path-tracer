#ifndef DELEGATE_HPP
#define DELEGATE_HPP

#include <vector>
#include <functional>

namespace ui
{

template <typename T1> 
class Delegate1
{
public:
	typedef void(FnType)(T1);

	void add(std::function<FnType>&& fn) {
		m_handlers.push_back(std::move(fn));
	}

	void remove(std::function<FnType>&& fn) {
		// TODO
		/*std::remove_if(m_handlers.begin(), m_handlers.end(), [](std::function<RT, Args...> const &handler) -> bool {
			if (handler.target() == fn.target()) {
				return true;
			}
		});*/
	}

	void operator()(T1 a1) {
		for (auto fn : m_handlers) {
			fn(a1);
		}
	}

private:
	std::vector<std::function<FnType> > m_handlers;
};

template <typename T1, typename T2> 
class Delegate2
{
public:
	typedef void(FnType)(T1, T2);
	void add(std::function<FnType>&& fn) {
		m_handlers.push_back(std::move(fn));
	}

	void remove(std::function<FnType>&& fn) {
	}

	void operator()(T1 a1, T2 a2) {
		for (auto fn : m_handlers) {
			fn(a1, a2);
		}
	}

private:
	std::vector<std::function<FnType> > m_handlers;
};

template <typename T1, typename T2, typename T3> 
class Delegate3
{
public:
	typedef void(FnType)(T1, T2, T3);

	void add(std::function<FnType>&& fn) {
		m_handlers.push_back(std::move(fn));
	}

	void remove(std::function<FnType>&& fn) {
	}

	void operator()(T1 a1, T2 a2, T3 a3) {
		for (auto fn : m_handlers) {
			fn(a1, a2, a3);
		}
	}

private:
	std::vector<std::function<FnType> > m_handlers;
};

}

#endif
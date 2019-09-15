#pragma once
#include <functional>
#include <vector>
#include <algorithm>
#include <iostream>

template<class ...EventArgs>
class Event
{
public:
	struct Subscriber {
		Subscriber(std::function<void(EventArgs...)> _function, long _id) :
			Function(_function),
			ID(_id) {}

		std::function<void(EventArgs...)> Function;
		long ID;
	};

	~Event();

	void Sub(std::function<void(EventArgs...)> func, void* _pClass);
	void UnSub(void* _pClass);
	void operator()(EventArgs... e);
private:
	std::hash<void*> m_Hasher;
	std::vector<Subscriber*> Subscribers;
};

template <class ... EventArgs>
Event<EventArgs...>::~Event()
{
	for (Subscriber* sub : Subscribers)
	{
		delete sub;
		sub = nullptr;
	}
}

template<class ...EventArgs>
inline void Event<EventArgs...>::Sub(std::function<void(EventArgs...)> func, void* _pClass)
{
	unsigned long id = m_Hasher(_pClass);
	Subscribers.push_back(new Subscriber(func, id));
}

template<class ...EventArgs>
inline void Event<EventArgs...>::UnSub(void* _pClass)
{
	const unsigned long subID = m_Hasher(_pClass);
	for (Subscriber* sub : Subscribers)
	{
		if (sub->ID == subID)
		{
			Subscribers.erase(std::remove(Subscribers.begin(), Subscribers.end(), sub), Subscribers.end());
			delete sub;
			break;
		}
	}
}

template<class ...EventArgs>
inline void Event<EventArgs...>::operator()(EventArgs... e)
{
	for (Subscriber* sub : Subscribers)
		sub->Function(e...);
}

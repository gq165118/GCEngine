#include "eventDispatcher.h"

namespace ff
{

	EventBase::EventBase(const std::string& eventName) noexcept
	{
		m_eventName = eventName;
	}

	EventBase::~EventBase() noexcept {}


	EventDispatcher::EventDispatcher() noexcept {}

	EventDispatcher::~EventDispatcher() noexcept {}

	EventDispatcher* EventDispatcher::getInstance()
	{
		if (m_instance == nullptr)
		{
			m_instance = new EventDispatcher();
		}

		return m_instance;
	}

	void EventDispatcher::dispatchEvent(const EventBase::Ptr& event)
	{
		const auto& eventName = event->m_eventName;

		auto listenerQueueIter = m_listeners.find(eventName);
		if (listenerQueueIter == m_listeners.end())
		{
			return;
		}

		auto& queue = listenerQueueIter->second;

		for (const auto& listener : queue)
		{
			//mFunction是已经bind好类对象（target）和类内函数指针(functionPointer),可以直接执行
			listener->m_function(event); 
		}
	}

}


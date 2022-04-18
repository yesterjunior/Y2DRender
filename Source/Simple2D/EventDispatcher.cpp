#include "Event.h"

namespace Simple2D
{
	EventDispatcher* pDispatcher = nullptr;

	//---------------------------------------------------------------------
	// EventListener
	// 事件监听器
	//---------------------------------------------------------------------
	unsigned int EventListener::nIDCounter = 0;

	EventListener::EventListener()
	{
		nID = nIDCounter++;
	}

	void EventListener::appendListener(int eventID, EventListener* new_listener)
	{
		auto listenerList = pDispatcher->listenerGroup.find(eventID);

		/* 事件 ID 没有监听列表？为 ID 创建监听列表，添加 eListener */
		if ( listenerList == pDispatcher->listenerGroup.end() ) {
			std::list<EventListener*> newListenerList;
			newListenerList.push_back(new_listener);
			pDispatcher->listenerGroup.insert(std::make_pair(eventID, newListenerList));
		}
		else {
			/* 如果监听列表中没有监听器，添加监听器到列表中 */
			std::list<EventListener*>::iterator listener_it;
			for ( listener_it = listenerList->second.begin(); listener_it != listenerList->second.end(); ++listener_it ) {
				if ( (*listener_it)->nID == new_listener->nID ) return;
			}
			if ( listener_it == listenerList->second.end() ) {
				listenerList->second.push_back(new_listener);
			}
		}
	}

	void EventListener::removeListener(int eventID, EventListener* listener)
	{
		auto listenerList = pDispatcher->listenerGroup.find(eventID);
		if ( listenerList == pDispatcher->listenerGroup.end() ) return;

		/* 从监听列表中移除监听器 */
		for ( auto it = listenerList->second.begin(); it != listenerList->second.end(); ++it ) {
			if ( (*it)->nID == listener->nID ) {
				listenerList->second.erase(it);
				break;
			}
		}
		/* 移除空监听列表 */
		if ( listenerList->second.empty() ) {
			pDispatcher->listenerGroup.erase(listenerList);
		}
	}


	//---------------------------------------------------------------------
	// EventDispatcher
	// 事件分派器
	//---------------------------------------------------------------------
	EventDispatcher EventDispatcher::eventDispatcher;

	EventDispatcher::EventDispatcher()
	{
		pDispatcher = this;
	}

	void EventDispatcher::dispatchEvent(const BaseEvent& event)
	{
		/* 只是暂时添加事件到事件池中，并没有立即分派事件，避免递归分派错误 */
		vEventPool.push_back(event);
	}

	void EventDispatcher::flushEvent()
	{
		if ( vEventPool.empty() ) return;

		/* 分派事件池中的所有事件 */
		for ( auto& event : vEventPool ) {
			this->realDispatchEvent(event);
		}
		vEventPool.clear();
	}

	void EventDispatcher::realDispatchEvent(const BaseEvent& event)
	{
		auto listenerList_it = listenerGroup.find(event.nEventID);
		if ( listenerList_it != listenerGroup.end() ) {
			std::list<EventListener*>& listenerList = listenerList_it->second;
			for ( auto listener_it : listenerList ) {
				listener_it->handleEvent(event);
			}
		}
	}
}
#include "Event.h"

namespace Simple2D
{
	EventDispatcher* pDispatcher = nullptr;

	//---------------------------------------------------------------------
	// EventListener
	// �¼�������
	//---------------------------------------------------------------------
	unsigned int EventListener::nIDCounter = 0;

	EventListener::EventListener()
	{
		nID = nIDCounter++;
	}

	void EventListener::appendListener(int eventID, EventListener* new_listener)
	{
		auto listenerList = pDispatcher->listenerGroup.find(eventID);

		/* �¼� ID û�м����б�Ϊ ID ���������б���� eListener */
		if ( listenerList == pDispatcher->listenerGroup.end() ) {
			std::list<EventListener*> newListenerList;
			newListenerList.push_back(new_listener);
			pDispatcher->listenerGroup.insert(std::make_pair(eventID, newListenerList));
		}
		else {
			/* ��������б���û�м���������Ӽ��������б��� */
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

		/* �Ӽ����б����Ƴ������� */
		for ( auto it = listenerList->second.begin(); it != listenerList->second.end(); ++it ) {
			if ( (*it)->nID == listener->nID ) {
				listenerList->second.erase(it);
				break;
			}
		}
		/* �Ƴ��ռ����б� */
		if ( listenerList->second.empty() ) {
			pDispatcher->listenerGroup.erase(listenerList);
		}
	}


	//---------------------------------------------------------------------
	// EventDispatcher
	// �¼�������
	//---------------------------------------------------------------------
	EventDispatcher EventDispatcher::eventDispatcher;

	EventDispatcher::EventDispatcher()
	{
		pDispatcher = this;
	}

	void EventDispatcher::dispatchEvent(const BaseEvent& event)
	{
		/* ֻ����ʱ����¼����¼����У���û�����������¼�������ݹ���ɴ��� */
		vEventPool.push_back(event);
	}

	void EventDispatcher::flushEvent()
	{
		if ( vEventPool.empty() ) return;

		/* �����¼����е������¼� */
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
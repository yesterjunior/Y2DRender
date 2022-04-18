#pragma once
#include "Common.h"

#include <map>
#include <list>

#define MAX_EVENT_PARAM 6

namespace Simple2D
{
	struct BaseEvent
	{
		int nEventID;					/* �¼� ID */
		int nParams[MAX_EVENT_PARAM];	/* �Զ������ */
		void* pUserData;				/* �û����� */
	};

	//---------------------------------------------------------------------
	// EventListener
	// �¼�������
	//---------------------------------------------------------------------
	class DLL_export EventListener
	{
		friend class EventDispatcher;

	public:
		EventListener();
		virtual ~EventListener() {}

	protected:
		void appendListener(int eventID, EventListener* listener);
		void removeListener(int eventID, EventListener* listener);

		virtual void handleEvent(const BaseEvent& event) = 0;

	private:
		static unsigned int nIDCounter;
		unsigned int nID;
	};


	//---------------------------------------------------------------------
	// EventDispatcher
	// �¼�������
	//---------------------------------------------------------------------
	class DLL_export EventDispatcher
	{
		friend class EventListener;
		typedef std::map<int, std::list<EventListener*>> ListenerGroup;

	public:
		EventDispatcher();
		static EventDispatcher* getInstance() { return &eventDispatcher; }

		void dispatchEvent(const BaseEvent& event);
		void flushEvent();

	private:
		void realDispatchEvent(const BaseEvent& event);

	private:
		static EventDispatcher eventDispatcher;

		std::list<BaseEvent> vEventPool;
		ListenerGroup listenerGroup;
	};
}
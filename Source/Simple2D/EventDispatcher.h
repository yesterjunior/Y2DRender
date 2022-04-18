#pragma once
#include "Common.h"

#include <map>
#include <list>

#define MAX_EVENT_PARAM 6

namespace Simple2D
{
	struct BaseEvent
	{
		int nEventID;					/* 事件 ID */
		int nParams[MAX_EVENT_PARAM];	/* 自定义参数 */
		void* pUserData;				/* 用户数据 */
	};

	//---------------------------------------------------------------------
	// EventListener
	// 事件监听器
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
	// 事件分派器
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
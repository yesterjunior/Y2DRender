#include "Event.h"

namespace Simple2D
{
	//------------------------------------------------------------------
	// MouseEventListener
	//------------------------------------------------------------------
	MouseEventListener::MouseEventListener()
	{
		this->appendListener(EventType::ET_MOUSE, this);
	}

	MouseEventListener::~MouseEventListener()
	{
		this->removeListener(EventType::ET_MOUSE, this);
	}

	void MouseEventListener::handleEvent(const BaseEvent& event)
	{
		if ( event.nEventID != EventType::ET_MOUSE && event.pUserData ) return;

		MouseEvent* mouseEvent = static_cast<MouseEvent*>(event.pUserData);

		switch ( mouseEvent->eventAction ) {
		case Simple2D::ACT_MOVE:		 this->mouseMove(*mouseEvent);		  break;
		case Simple2D::ACT_PRESS:		 this->mousePressed(*mouseEvent);	  break;
		case Simple2D::ACT_RELAESE:		 this->mouseReleased(*mouseEvent);	  break;
		case Simple2D::ACT_SCROLL:		 this->mouseWheel(*mouseEvent);		  break;
		case Simple2D::ACT_DUBBLE_CLICK: this->mouseDoubleClick(*mouseEvent); break;
		}
	}


	//------------------------------------------------------------------
	// KeyEventListener
	//------------------------------------------------------------------
	KeyEventListener::KeyEventListener()
	{
		this->appendListener(EventType::ET_KEY, this);
	}

	KeyEventListener::~KeyEventListener()
	{
		this->removeListener(EventType::ET_KEY, this);
	}

	void KeyEventListener::handleEvent(const BaseEvent& event)
	{
		if ( event.nEventID != EventType::ET_KEY && event.pUserData ) return;

		KeyEvent* keyEvent = static_cast<KeyEvent*>(event.pUserData);

		switch ( keyEvent->eventAction ) {
		case Simple2D::ACT_PRESS:	this->keyPressed(*keyEvent);	  break;
		case Simple2D::ACT_RELAESE:	this->keyReleased(*keyEvent); break;
		}
	}


	//------------------------------------------------------------------
	// WinMsgHandle
	// 窗口信息处理
	//------------------------------------------------------------------
	WinMsgHandle::WinMsgHandle()
	{
		memset(&baseEvent, 0, sizeof(BaseEvent));

		memset(&keyEvent, 0, sizeof(KeyEvent));
		memset(&mouseEvent, 0, sizeof(MouseEvent));
	}

	void WinMsgHandle::handleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		baseEvent.nEventID = ET_UNKNOWN;

		/* 鼠标事件信息  */
		if ( msg >= WM_MOUSEMOVE && msg <= WM_MBUTTONDBLCLK || msg == WM_MOUSEWHEEL ) {
			switch ( msg ) {
			case WM_LBUTTONDOWN:
				mouseEvent.buttonType = ButtonType::LEFT_BUTTON;
				mouseEvent.eventAction = EventAction::ACT_PRESS;
				break;
			case WM_LBUTTONUP:
				mouseEvent.buttonType = ButtonType::LEFT_BUTTON;
				mouseEvent.eventAction = EventAction::ACT_RELAESE;
				break;
			case WM_LBUTTONDBLCLK:
				mouseEvent.buttonType = ButtonType::LEFT_BUTTON;
				mouseEvent.eventAction = EventAction::ACT_DUBBLE_CLICK;
				break;
			case WM_MBUTTONDOWN:
				mouseEvent.buttonType = ButtonType::MIDDLE_BUTTON;
				mouseEvent.eventAction = EventAction::ACT_PRESS;
				break;
			case WM_MBUTTONUP:
				mouseEvent.buttonType = ButtonType::MIDDLE_BUTTON;
				mouseEvent.eventAction = EventAction::ACT_RELAESE;
				break;
			case WM_MBUTTONDBLCLK:
				mouseEvent.buttonType = ButtonType::MIDDLE_BUTTON;
				mouseEvent.eventAction = EventAction::ACT_DUBBLE_CLICK;
				break;
			case WM_RBUTTONDOWN:
				mouseEvent.buttonType = ButtonType::RIGHT_BUTTON;
				mouseEvent.eventAction = EventAction::ACT_PRESS;
				break;
			case WM_RBUTTONUP:
				mouseEvent.buttonType = ButtonType::RIGHT_BUTTON;
				mouseEvent.eventAction = EventAction::ACT_RELAESE;
				break;
			case WM_RBUTTONDBLCLK:
				mouseEvent.buttonType = ButtonType::RIGHT_BUTTON;
				mouseEvent.eventAction = EventAction::ACT_DUBBLE_CLICK;
				break;
			case WM_MOUSEMOVE:
				mouseEvent.eventAction = EventAction::ACT_MOVE;
				break;
			case WM_MOUSEWHEEL:
				mouseEvent.eventAction = EventAction::ACT_SCROLL;
				mouseEvent.nDelta = ( short ) HIWORD(wParam);
				break;
			}
			mouseEvent.nX = ( short ) LOWORD(lParam);
			mouseEvent.nY = ( short ) HIWORD(lParam);
			baseEvent.nEventID = ET_MOUSE;
			baseEvent.pUserData = &mouseEvent;
			EventDispatcher::getInstance()->dispatchEvent(baseEvent);
		}

		/* 键盘按键事件信息 */
		if ( msg == WM_KEYDOWN || msg == WM_KEYUP ) {
			keyEvent.eventAction = (msg == WM_KEYDOWN) ? EventAction::ACT_PRESS : EventAction::ACT_RELAESE;
			keyEvent.keyType = keyMap(( UINT ) wParam);
			keyEvent.keys[( UINT ) wParam] = (msg == WM_KEYDOWN) ? true : false;

			baseEvent.nEventID = ET_KEY;
			baseEvent.pUserData = &keyEvent;
			EventDispatcher::getInstance()->dispatchEvent(baseEvent);
		}
	}
}
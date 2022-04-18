#pragma once
#include "EventDispatcher.h"

#include <map>
#include <list>

#include <Windows.h>

namespace Simple2D
{
	enum EventType 
	{ 
		ET_UNKNOWN,			/* 未知事件 */
		ET_MOUSE,			/* 鼠标事件 */
		ET_KEY				/* 按键事件 */
	};

	enum ButtonType 
	{ 
		LEFT_BUTTON,		/* 鼠标左键 */
		RIGHT_BUTTON,		/* 鼠标右键 */
		MIDDLE_BUTTON		/* 鼠标中键 */
	};

	enum EventAction
	{
		ACT_MOVE,			/* 移动 */
		ACT_PRESS,			/* 按压 */
		ACT_RELAESE,		/* 释放 */
		ACT_DUBBLE_CLICK,	/* 双击 */
		ACT_SCROLL			/* 滚动 */
	};


	/*
	* VK_0 - VK_9 are the same as ASCII '0' - '9' (0x30 - 0x39)
	* 0x40 : unassigned
	* VK_A - VK_Z are the same as ASCII 'A' - 'Z' (0x41 - 0x5A)
	*/
	enum KeyType
	{
		Key_Unknown,

		Key_Space = 0x20,
		Key_Prior,
		Key_Next,
		Key_End,
		Key_Home,
		Key_Left,
		Key_Up,
		Key_Right,
		Key_Down,
		Key_Select,
		Key_Print,
		Key_Execute,
		Key_Snapshot,
		Key_Insert,
		Key_Delete,
		Key_Help,

		/* 主键盘上的数字键 */
		Key_0 = 0x30,
		Key_1,
		Key_2,
		Key_3,
		Key_4,
		Key_5,
		Key_6,
		Key_7,
		Key_8,
		Key_9,

		Key_A = 0x41,
		Key_B,
		Key_C,
		Key_D,
		Key_E,
		Key_F,
		Key_G,
		Key_H,
		Key_I,
		Key_J,
		Key_K,
		Key_L,
		Key_M,
		Key_N,
		Key_O,
		Key_P,
		Key_Q,
		Key_R,
		Key_S,
		Key_T,
		Key_U,
		Key_V,
		Key_W,
		Key_X,
		Key_Y,
		Key_Z,

		/* 小键盘上的数字 */
		Key_NumPad_0 = 0x60,
		Key_NumPad_1,
		Key_NumPad_2,
		Key_NumPad_3,
		Key_NumPad_4,
		Key_NumPad_5,
		Key_NumPad_6,
		Key_NumPad_7,
		Key_NumPad_8,
		Key_NumPad_9,

		Key_F1 = 0x70,
		Key_F2,
		Key_F3,
		Key_F4,
		Key_F5,
		Key_F6,
		Key_F7,
		Key_F8,
		Key_F9,
		Key_F10,
		Key_F11,
		Key_F12,
		Key_F13,
		Key_F14,
		Key_F15,
		Key_F16,
		Key_F17,
		Key_F18,
		Key_F19,
		Key_F20,
		Key_F21,
		Key_F22,
		Key_F23,
		Key_F24,
	};

	static KeyType keyMap(unsigned int key)
	{
		return KeyType(key);
	}

	/* 鼠标事件  */
	struct MouseEvent
	{
		EventAction eventAction;
		ButtonType buttonType;
		int nDelta;
		int nX, nY;
	};

	/* 按键事件 */
	struct KeyEvent
	{
		EventAction eventAction;
		bool keys[256];
		KeyType keyType;
	};

	//------------------------------------------------------------------
	// MouseEventListener
	//------------------------------------------------------------------
	class DLL_export MouseEventListener : public EventListener
	{
	public:
		MouseEventListener();
		virtual ~MouseEventListener();

		virtual void mouseMove(const MouseEvent& event) {}
		virtual void mousePressed(const MouseEvent& event) {}
		virtual void mouseReleased(const MouseEvent& event) {}
		virtual void mouseDoubleClick(const MouseEvent& event) {}
		virtual void mouseWheel(const MouseEvent& event) {}

		void handleEvent(const BaseEvent& event);
	};


	//------------------------------------------------------------------
	// KeyEventListener
	//------------------------------------------------------------------
	class DLL_export KeyEventListener : public EventListener
	{
	public:
		KeyEventListener();
		virtual ~KeyEventListener();

		virtual void keyPressed(const KeyEvent& event) {}
		virtual void keyReleased(const KeyEvent& event) {}

		void handleEvent(const BaseEvent& event);
	};


	//------------------------------------------------------------------
	// WinMsgHandle
	// 窗口信息处理
	//------------------------------------------------------------------
	class WinMsgHandle
	{
	public:
		WinMsgHandle();

		void handleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	private:
		BaseEvent baseEvent;

		KeyEvent keyEvent;
		MouseEvent mouseEvent;
	};
}
#pragma once
#include "EventDispatcher.h"

#include <map>
#include <list>

#include <Windows.h>

namespace Simple2D
{
	enum EventType 
	{ 
		ET_UNKNOWN,			/* δ֪�¼� */
		ET_MOUSE,			/* ����¼� */
		ET_KEY				/* �����¼� */
	};

	enum ButtonType 
	{ 
		LEFT_BUTTON,		/* ������ */
		RIGHT_BUTTON,		/* ����Ҽ� */
		MIDDLE_BUTTON		/* ����м� */
	};

	enum EventAction
	{
		ACT_MOVE,			/* �ƶ� */
		ACT_PRESS,			/* ��ѹ */
		ACT_RELAESE,		/* �ͷ� */
		ACT_DUBBLE_CLICK,	/* ˫�� */
		ACT_SCROLL			/* ���� */
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

		/* �������ϵ����ּ� */
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

		/* С�����ϵ����� */
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

	/* ����¼�  */
	struct MouseEvent
	{
		EventAction eventAction;
		ButtonType buttonType;
		int nDelta;
		int nX, nY;
	};

	/* �����¼� */
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
	// ������Ϣ����
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
#include "InputManager.h"
#include "Window.h"

namespace AlbinoEngine
{
	void InputManager::update(Window& window)
	{
		m_mouseDeltaX = 0;
		m_mouseDeltaY = 0;

		HWND hwnd = window.getWindowHandle();
		if (!hwnd)
		{
			resetMouseData();
			return;
		}

		// Only respond when this window is the active foregroubnd window.
		if (!window.hasFocus())
		{
			resetMouseData();
			return;
		}

		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(hwnd, &pt);

		m_leftMouseDown = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;

		if (m_leftMouseDown)
		{
			if (!m_hasLastMousePos)
			{
				m_lastMousePos = pt;
				m_hasLastMousePos = true;
			}
			else
			{
				m_mouseDeltaX = pt.x - m_lastMousePos.x;
				m_mouseDeltaY = pt.y - m_lastMousePos.y;
				m_lastMousePos = pt;
			}
		}
		else
		{
			m_hasLastMousePos = false;
		}
	}

	void InputManager::resetMouseData()
	{
		m_leftMouseDown = false;
		m_mouseDeltaX = 0;
		m_mouseDeltaY = 0;
		m_hasLastMousePos = false;
	}
}
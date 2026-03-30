#pragma once

#include <Windows.h>

namespace AlbinoEngine
{
	class Window;

	class InputManager
	{
	public:
		void update(Window& window);
		void resetMouseData();

		bool isLeftMouseDown() const { return m_leftMouseDown; }
		int getMouseDeltaX() const { return m_mouseDeltaX; }
		int getMouseDeltaY() const { return m_mouseDeltaY; }

	private:
		bool m_leftMouseDown = false;

		int m_mouseDeltaX = 0;
		int m_mouseDeltaY = 0;

		POINT m_lastMousePos{ 0,0 };
		bool m_hasLastMousePos = false;
	};
}
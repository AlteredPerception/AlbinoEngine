#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <string>

namespace AlbinoEngine
{
	class Window
	{
	public:
		Window(); // Default.

		Window(
			std::wstring winTitle, // Window title.
			UINT width,			  // Window width.
			UINT height			  // Window height
		);

		bool createWindow();

		bool createWindow(std::wstring winTitle = L"Albino Engine", UINT width = 800, UINT height = 600);

		void show(); 

		void hide();

		bool isHidden() const;

		//WNDPROC getWinProc() const { return (WNDPROC)winProc; }

		HWND getWindowHandle() const { return m_pWinHandle; }

		UINT getWidth() const { return m_pWidth; }

		UINT getHeight() const { return m_pHeight; }

		bool hasFocus() const { return m_hasFocus; }

		void setFocus(bool hasFocus) { m_hasFocus = hasFocus; }

	protected:
		bool m_hasFocus = false;
		static LRESULT CALLBACK winProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		LRESULT HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		bool registerWindow();
		HWND createWindowImp(
			std::wstring winTitle,
			UINT width,
			UINT height);
		HWND m_pWinHandle;
		std::wstring m_pWinTitle;
		UINT m_pWidth;
		UINT m_pHeight;
	};
}
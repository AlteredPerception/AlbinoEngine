#include "Window.h"

namespace AlbinoEngine
{
	Window::Window()
		: m_pWinTitle(L"Albino Engine"),
		m_pWidth(1),
		m_pHeight(1),
		m_pWinHandle(0)
	{
	
	}

	Window::Window(std::wstring winTitle, UINT width, UINT height)
		: m_pWinTitle(winTitle), 
		m_pWidth(width), 
		m_pHeight(height), 
		m_pWinHandle(0)
	{

	}

	bool Window::registerWindow()
	{
		WNDCLASSEX winClassEx = {};
		ZeroMemory(&winClassEx, sizeof(winClassEx));

		winClassEx.cbSize = sizeof(WNDCLASSEX);

		winClassEx.style = CS_HREDRAW | CS_VREDRAW;
		winClassEx.lpfnWndProc = (WNDPROC)winProc;
		winClassEx.hInstance = (HINSTANCE)GetModuleHandle(NULL);
		winClassEx.hCursor = LoadCursor(NULL, IDC_ARROW);
		winClassEx.hIcon = LoadIcon(NULL, IDI_WINLOGO);
		winClassEx.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		winClassEx.lpszClassName = this->m_pWinTitle.c_str();

	    return RegisterClassEx(&winClassEx);
	}

	bool Window::createWindow()
	{
		return true;
	}

	bool Window::createWindow(std::wstring winTitle, UINT width, UINT height)
	{
		m_pWinTitle = winTitle;
		m_pWidth = width;
		m_pHeight = height;
		if (this->registerWindow()) {
			m_pWinHandle = this->createWindowImp(m_pWinTitle, m_pWidth, m_pHeight);
		}
		return m_pWinHandle != NULL;
	}

	HWND Window::createWindowImp(std::wstring winTitle, UINT width, UINT height)
	{
		DWORD dwExStyle = WS_EX_OVERLAPPEDWINDOW;
		DWORD dwStyle   = WS_OVERLAPPEDWINDOW;
		RECT winRect = {};
		winRect.left = 0;
		winRect.top = 0;
		winRect.right = width;
		winRect.bottom = height;

		AdjustWindowRectEx(&winRect, dwStyle, NULL, dwExStyle);

		return CreateWindowEx(
			dwExStyle, 
			this->m_pWinTitle.c_str(), 
			this->m_pWinTitle.c_str(), 
			dwStyle, 
			0, 
			0, 
			winRect.right - winRect.left, 
			winRect.bottom - winRect.top, 
			NULL, 
			NULL, 
			GetModuleHandle(NULL),
			NULL);
	}

	void Window::show()
	{
		if (m_pWinHandle)
		{
			ShowWindow(m_pWinHandle, SW_SHOW);
			UpdateWindow(m_pWinHandle);
		}
	}

	void Window::hide()
	{
		if (m_pWinHandle)
		{
			ShowWindow(m_pWinHandle, SW_HIDE);
			UpdateWindow(m_pWinHandle);
		}
	}

	bool Window::isHidden() const
	{
		return (IsWindowVisible(this->m_pWinHandle));
	}

	LRESULT WINAPI Window::winProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_QUIT:
			DestroyWindow(hWnd);
			break;
		case WM_CLOSE:
			PostQuitMessage(0);
			break;
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}
#include "../includes/Window.h"
#include "../includes/Log.h"

namespace Cube
{
	Window::WindowClass Window::WindowClass::wndClass;
	Window::WindowClass::WindowClass() 
	{
		WNDCLASSEX wc = { 0 };
		wc.cbSize = sizeof(wc);
		wc.style = CS_OWNDC;
		wc.lpfnWndProc = HandleMessageSetup;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = getInstance();
		wc.hIcon = nullptr;
		wc.hCursor = nullptr;
		wc.hbrBackground = nullptr;
		wc.lpszMenuName = nullptr;
		wc.lpszClassName = getName();
		wc.hIconSm = nullptr;
		RegisterClassEx(&wc);
	}

	Window::WindowClass::~WindowClass()
	{
		UnregisterClass(wndClassName, getInstance());
	}

	const wchar_t* Window::WindowClass::getName()
	{
		return wndClassName;
	}

	HINSTANCE Window::WindowClass::getInstance()
	{
		return wndClass.hInst;
	}

	Window::Window(int width, int height)
	{
		RECT wr;
		wr.left = 100;
		wr.top = 100;
		wr.right = width + wr.left;
		wr.bottom = height + wr.top;
		AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU, FALSE);
		hwnd = CreateWindowEx(0, WindowClass::getName(), L"Cube Engine", WS_CAPTION | WS_MINIMIZEBOX | WS_SIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU,
							CW_USEDEFAULT, CW_USEDEFAULT, width, height, nullptr, nullptr, WindowClass::getInstance(), this);
		CUBE_CORE_INFO("Window was created.");
	}
	Window::~Window()
	{
		DestroyWindow(hwnd);
	}

	void Window::WindowShow()
	{
		CUBE_CORE_INFO("Window was shown.");
		ShowWindow(hwnd, SW_SHOW);
	}
	
	LRESULT WINAPI Window::HandleMessageSetup(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (message == WM_NCCREATE)
		{
			const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
			Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
			SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
			SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMessageThunk));
			return pWnd->HandleMessage(hwnd, message, wParam, lParam);
		}
		return DefWindowProc(hwnd, message, wParam, lParam);
	}

	LRESULT WINAPI Window::HandleMessageThunk(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
		return pWnd->HandleMessage(hwnd, message, wParam, lParam);
	}

	LRESULT Window::HandleMessage(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_CLOSE:
			CUBE_CORE_INFO("Window was closed.");
			PostQuitMessage(0);
			return 0;
		}
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
}
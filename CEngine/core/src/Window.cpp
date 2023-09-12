#include "../includes/Window.h"
#include "../includes/Log.h"

namespace Cube
{
	Window::Window(HINSTANCE hInstance)
	{
		wc = { 0 };
		wc.cbSize = sizeof(wc);
		wc.style = CS_OWNDC;
		wc.lpfnWndProc = DefWindowProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hInstance;
		wc.hIcon = nullptr;
		wc.hCursor = nullptr;
		wc.hbrBackground = nullptr;
		wc.lpszMenuName = nullptr;
		wc.lpszClassName = pClassName;
		wc.hIconSm = nullptr;
		RegisterClassEx(&wc);
		hwnd = CreateWindowEx(0, pClassName, L"Cube Engine", WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
									200, 200, 1280, 720, nullptr, nullptr, hInstance, nullptr);
		CUBE_CORE_INFO("Window was created");
	}

	void Window::WindowShow()
	{
		CUBE_CORE_INFO("Window was shown");
		ShowWindow(hwnd, SW_SHOW);
	}

	Window::~Window()
	{
	
	}
}
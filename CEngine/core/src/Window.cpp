#include "../includes/Window.h"
#include "../includes/Log.h"
#include <sstream>

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
		wc.hIcon = (HICON)LoadImage(getInstance(), L"C:\\Users\\user999\\source\\repos\\CubeEngine\\kubik2.ico", IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
		wc.hCursor = nullptr;
		wc.hbrBackground = nullptr;
		wc.lpszMenuName = nullptr;
		wc.lpszClassName = getName();
		wc.hIconSm = (HICON)LoadImage(getInstance(), L"C:\\Users\\user999\\source\\repos\\CubeEngine\\kubik.ico", IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
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
		if (FAILED(AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU, FALSE)))
		{
			throw CUBE_LAST_EXCEPTION();
		};
		hwnd = CreateWindowEx(0, WindowClass::getName(), L"Cube Engine", WS_CAPTION | WS_MINIMIZEBOX | WS_SIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU,
							CW_USEDEFAULT, CW_USEDEFAULT, width, height, nullptr, nullptr, WindowClass::getInstance(), this);
		if (hwnd == nullptr)
		{
			throw CUBE_LAST_EXCEPTION();
		};
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
	Window::Exception::Exception(int line, const char* file, HRESULT hResult) : CubeException(line, file), hResult(hResult)
	{

	}
	const char* Window::Exception::whatEx() const
	{
		std::ostringstream oss;
		oss << getType() << std::endl << "[Error code] " << GetErrorCode() << std::endl << "[Description] " << GetErrorString() << std::endl << getOriginString();
		whatExBuffer = oss.str();
		return whatExBuffer.c_str();
	}

	const char* Window::Exception::getType() const
	{
		return "Cube Window Exception";
	}

	std::string Window::Exception::TranslateErrorCode(HRESULT hResult)
	{
		char* pMessageBuffer = nullptr;
		DWORD nMessageLen = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 
										  nullptr, hResult, MAKELANGID(LANG_NEPALI, SUBLANG_DEFAULT), reinterpret_cast<wchar_t*>(&pMessageBuffer), 0, nullptr);
		if (nMessageLen == 0)
		{
			return "Unidentified error code.";
		}
		std::string errorString = pMessageBuffer;
		LocalFree(pMessageBuffer);
		return errorString;
	}
	
	HRESULT Window::Exception::GetErrorCode() const
	{
		return hResult;
	}

	std::string Window::Exception::GetErrorString() const
	{
		return TranslateErrorCode(hResult);
	}
}
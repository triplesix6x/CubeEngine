//Файл инициализации и настройки окна

#include "../includes/Log.h"
#include <sstream>
#include "../includes/Window.h"

namespace Cube
{
	Window::WindowClass Window::WindowClass::wndClass;
	Window::WindowClass::WindowClass() 
	{
		//Создание и настройка дескриптора класса окна
		WNDCLASSEX wc = { 0 };
		wc.cbSize = sizeof(wc);
		wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = HandleMessageSetup;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = getInstance();
		wc.hIcon = (HICON)LoadImage(getInstance(), L"icons\\kubik2.ico", 
													IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
		wc.hCursor = (HCURSOR)LoadImage(getInstance(), L"icons\\cursor.ico", 
													IMAGE_ICON, 24, 24, LR_LOADFROMFILE);
		wc.hbrBackground = nullptr;
		wc.lpszMenuName = nullptr;
		wc.lpszClassName = getName();
		wc.hIconSm = (HICON)LoadImage(getInstance(), L"icons\\kubik.ico", 
													IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
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

	Window::Window(int width, int height) : width(width), height(height)
	{
		//Создание и настройка окна
		RECT wr;
		wr.left = 100;
		wr.top = 100;
		wr.right = width + wr.left;
		wr.bottom = height + wr.top;
		if (AdjustWindowRect(&wr, WindowType::FULL, FALSE) == 0)
		{
			throw CUBE_LAST_EXCEPTION();
		};

		RECT desktop;
		const HWND hDesktop = GetDesktopWindow();
		GetWindowRect(hDesktop, &desktop);

		hwnd = CreateWindowEx(0, WindowClass::getName(), L"Cube Engine", WindowType::FULL,
							((desktop.right / 2) - (width / 2)), ((desktop.bottom / 2) - (height / 2)), 
							width, height, nullptr, nullptr, WindowClass::getInstance(), this);
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
		//Отображение окна и запуск конструктора Graphics
		CUBE_CORE_INFO("Window was shown.");
		ShowWindow(hwnd, SW_SHOW);
		pGfx = std::make_unique<Graphics>(hwnd, width, height);
	}
	
	
	//Ниже - обработка исключений окна
	Window::Exception::Exception(int line, const char* file, HRESULT hResult) : CubeException(line, file), hResult(hResult)
	{

	}
	const char* Window::Exception::whatEx() const
	{
		std::ostringstream oss;
		oss << getType() << std::endl << "[Error code] " << GetErrorCode() << std::endl << "[Description] " << GetErrorString() << std::endl << getOriginString();
		whatExBuffer = oss.str();
		CUBE_CORE_ERROR("Cube Exception was thrown -> \n{} {}", whatExBuffer.c_str(), getType());
		return whatExBuffer.c_str();
	}

	inline const char* Window::Exception::getType() const
	{
		return "Cube Window Exception";
	}

	const char* Window::NoGfxException::getType() const
	{
		return "Chili Window Exception [No Graphics]";
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

	//Функция обработки сообщений внутри окна
	std::optional<int> Window::ProcessMessages()
	{
		MSG message;
		while (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
		{
			if (message.message == WM_QUIT)
			{
				return message.wParam;
			}
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		return {};
	}
	//Функция возвращения указателя на экземпляр Graphics
	Graphics& Window::Gfx()
	{
		if(!pGfx)
		{
			throw HWND_NOGFX_EXCEPTION();
		}
		return *pGfx;
	}

	//Ниже - обработка сообщений окна
	LRESULT WINAPI Window::HandleMessageSetup(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (message == WM_NCCREATE)
		{
			const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
			Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
			SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
			SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMessageThunk));
			pWnd->handle(hwnd);
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
		case WM_MOUSEMOVE:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			if(pt.x >= 0 && pt.x < width && pt.y >= 0 && pt.y < height)
			{ 
				mouse.OnMouseMove(pt.x, pt.y);
				if (!mouse.IsInWindow())
				{
					SetCapture(handle());
					mouse.OnMouseEnter();
				}
			}
			else
			{
				if (wParam & (MK_LBUTTON | MK_RBUTTON))
				{
					mouse.OnMouseMove(pt.x, pt.y);
				}
				else
				{
					ReleaseCapture();
					mouse.OnMouseLeave();
				}
			}
			
			break;
		}
		case WM_LBUTTONDOWN:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			mouse.OnLeftPressed(pt.x, pt.y);
			break;
		}
		case WM_RBUTTONDOWN:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			mouse.OnRightPressed(pt.x, pt.y);
			break;
		}
		case WM_LBUTTONUP:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			mouse.OnLeftReleased(pt.x, pt.y);
			break;
		}
		case WM_RBUTTONUP:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			mouse.OnRightReleased(pt.x, pt.y);
			break;
		}
		case WM_MOUSEWHEEL:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
			{
				mouse.OnWheelUp(pt.x, pt.y);
			}
			else if (GET_WHEEL_DELTA_WPARAM(wParam) < 0)
			{
				mouse.OnWheelDown(pt.x, pt.y);
			}
		}

		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
			if (!(lParam & 0x40000000) || kbd.AutorepeatIsEnabled())
			{
				kbd.OnKeyPressed(static_cast<unsigned char>(wParam));
			}
			break;
		case WM_KEYUP:
		case WM_SYSKEYUP:
			kbd.OnKeyReleased(static_cast<unsigned char>(wParam));
			break;
		case WM_CHAR:
			kbd.OnChar(static_cast<unsigned char>(wParam));
			break;
		case WM_KILLFOCUS:
			kbd.ClearState();
			break;
		case WM_CLOSE:
			CUBE_CORE_INFO("Window was closed.");
			PostQuitMessage(0);
			return 0;
		}
		return DefWindowProc(hwnd, message, wParam, lParam);
	}

}
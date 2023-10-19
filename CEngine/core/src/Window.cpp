//Файл инициализации и настройки окна

#include "../includes/Log.h"
#include <sstream>
#include "../includes/Window.h"
#include "../../imgui/imgui.h"
#include "../../imgui/imgui_impl_win32.h"


namespace Cube
{
	Window::WindowClass Window::WindowClass::wndClass;
	Window::WindowClass::WindowClass()
	{
		//Создание и настройка дескриптора класса окна
		WNDCLASSEX wc = { 0 };
		wc.cbSize = sizeof(wc);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = HandleMessageSetup;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = getInstance();
		wc.hIcon = (HICON)LoadImage(getInstance(), L"icons\\kubik2.ico",
			IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
		wc.hCursor = nullptr;
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

	Window::Window(int width, int height, WindowType type) : width(width), height(height), type(type)
	{
		//Создание и настройка окна
		RECT wr;
		wr.left = 0;
		wr.top = 0;
		wr.right = width;
		wr.bottom = height;
		if (AdjustWindowRect(&wr, type, FALSE) == 0)
		{
			throw CUBE_LAST_EXCEPTION();
		};

		RECT desktop;
		const HWND hDesktop = GetDesktopWindow();
		GetWindowRect(hDesktop, &desktop);

		hwnd = CreateWindowEx(0, WindowClass::getName(), L"Cube Engine", type,
			((desktop.right / 2) - (width / 2)), ((desktop.bottom / 2) - (height / 2)),
			wr.right - 50, wr.bottom - 50, nullptr, nullptr, WindowClass::getInstance(), this);
		if (hwnd == nullptr)
		{
			throw CUBE_LAST_EXCEPTION();
		};
		//Отображение окна,ImGui и конструктора Graphics
		ShowWindow(hwnd, SW_SHOWDEFAULT);
		CUBE_CORE_INFO("Window was shown.");
		ImGui_ImplWin32_Init(hwnd);
		CUBE_CORE_INFO("ImGui Win32 was initialized.");
		pGfx = std::make_unique<Graphics>(hwnd, width, height);
		CUBE_CORE_INFO("Window was created.");

		RAWINPUTDEVICE rid;
		rid.usUsagePage = 0x01;
		rid.usUsage = 0x02;
		rid.dwFlags = 0;
		rid.hwndTarget = nullptr;
		if (RegisterRawInputDevices(&rid, 1, sizeof(rid)) == FALSE)
		{
			throw CUBE_LAST_EXCEPTION();
		}
	}

	int Window::GetWidth() const noexcept
	{
		return width;
	}

	int Window::GetHeight() const noexcept
	{
		return height;
	}

	void Window::EnableCursor()
	{
		cursorEnabled = true;
		ShowCursor();
		EnableImGuiCursor();
		FreeCursor();
	}
	
	void Window::DisableCursor()
	{
		cursorEnabled = false;
		HideCursor();
		DisableImGuiCursor();
		ConfineCursor();
	}

	void Window::HideCursor()
	{
		while (::ShowCursor(FALSE) >= 0);
	}
	
	void Window::ShowCursor()
	{
		while (::ShowCursor(TRUE) < 0);
	}

	void Window::EnableImGuiCursor()
	{
		ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
	}

	void Window::DisableImGuiCursor()
	{
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
	}

	void Window::ConfineCursor()
	{
		RECT rect;
		GetClientRect(hwnd, &rect);
		MapWindowPoints(hwnd, nullptr, reinterpret_cast<POINT*>(&rect), 2);
		ClipCursor(&rect);
	}

	void Window::FreeCursor()
	{
		ClipCursor(nullptr);
	}

	Window::~Window()
	{
		ImGui_ImplWin32_Shutdown();
		DestroyWindow(hwnd);
	}
	
	static bool win32_window_is_maximized(HWND handle) 
	{
		WINDOWPLACEMENT placement = { 0 };
		placement.length = sizeof(WINDOWPLACEMENT);
		if (GetWindowPlacement(handle, &placement)) 
		{
			return placement.showCmd == SW_SHOWMAXIMIZED;
		}
		return false;
	}

	//Ниже - обработка исключений окна
	Window::Exception::Exception(int line, const char* file, HRESULT hResult) : CubeException(line, file), hResult(hResult)
	{
		MessageBoxA(nullptr, whatEx(), getType(), MB_OK | MB_ICONEXCLAMATION);
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
	std::optional<MSG> Window::ProcessMessages()
	{
		MSG message;
		while (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
		{
			if (message.message == WM_QUIT)
			{
				return message;
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
		if (ImGui_ImplWin32_WndProcHandler(hwnd, message, wParam, lParam))
		{
			return true;
		}
		const auto imguiio = ImGui::GetIO();
		switch (message)
		{
		case WM_INPUT:
		{
			UINT size;
			if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER)) == -1)
			{
				break;
			}
			rawBuffer.resize(size);
			if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, rawBuffer.data(), &size, sizeof(RAWINPUTHEADER)) != size)
			{
				break;
			}
			auto& ri = reinterpret_cast<const RAWINPUT&>(*rawBuffer.data());
			if (ri.header.dwType == RIM_TYPEMOUSE && (ri.data.mouse.lLastX != 0 || ri.data.mouse.lLastY != 0))
			{
				mouse.OnRawDelta(ri.data.mouse.lLastX, ri.data.mouse.lLastY);
			}
			break;
		}
		case WM_MOUSEMOVE:
		{
			if (imguiio.WantCaptureMouse)
			{
				break;
			}
			const POINTS pt = MAKEPOINTS(lParam);
			if (pt.x >= 0 && pt.x < width && pt.y >= 0 && pt.y < height)
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
			if (imguiio.WantCaptureMouse)
			{
				break;
			}
			const POINTS pt = MAKEPOINTS(lParam);
			mouse.OnLeftPressed(pt.x, pt.y);
			break;
		}
		case WM_RBUTTONDOWN:
		{
			if (imguiio.WantCaptureMouse)
			{
				break;
			}
			const POINTS pt = MAKEPOINTS(lParam);
			mouse.OnRightPressed(pt.x, pt.y);
			break;
		}
		case WM_LBUTTONUP:
		{
			if (imguiio.WantCaptureMouse)
			{
				break;
			}
			const POINTS pt = MAKEPOINTS(lParam);
			mouse.OnLeftReleased(pt.x, pt.y);
			break;
		}
		case WM_RBUTTONUP:
		{
			if (imguiio.WantCaptureMouse)
			{
				break;
			}
			const POINTS pt = MAKEPOINTS(lParam);
			mouse.OnRightReleased(pt.x, pt.y);
			break;
		}
		case WM_MOUSEWHEEL:
		{
			if (imguiio.WantCaptureMouse)
			{
				break;
			}
			const POINTS pt = MAKEPOINTS(lParam);
			if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
			{
				mouse.OnWheelUp(pt.x, pt.y);
			}
			else if (GET_WHEEL_DELTA_WPARAM(wParam) < 0)
			{
				mouse.OnWheelDown(pt.x, pt.y);
			}
			break;
		}
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		{
			if (!(lParam & 0x40000000) || kbd.AutorepeatIsEnabled())
			{
				kbd.OnKeyPressed(static_cast<unsigned char>(wParam));
			}
			break;
		}
		case WM_KEYUP:
		case WM_SYSKEYUP:
		{
			kbd.OnKeyReleased(static_cast<unsigned char>(wParam));
			break;
		}
		case WM_CHAR:
			kbd.OnChar(static_cast<unsigned char>(wParam));
			break;
		case WM_KILLFOCUS:
			kbd.ClearState();
			break;
		case WM_SIZE:
			if (wParam == SIZE_MINIMIZED)
				return 0;
			if (wParam == SIZE_MAXIMIZED) 
			{
				if (type == CUSTOM) 
				{
					if (pGfx) 
					{
						Gfx().Resize(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
					}
					return 0;
				}
			}
			if (pGfx)
			{
				Gfx().Resize((UINT)LOWORD(lParam), (UINT)HIWORD(lParam));
			}
			break;
		case WM_ACTIVATE:
		{
			if (!cursorEnabled)
			{
				if (wParam & WA_ACTIVE || wParam & WA_CLICKACTIVE)
				{
					ConfineCursor();
					HideCursor();
				}
				else
				{
					FreeCursor();
					ShowCursor();
				}
			}
			break;
		}
			case WM_NCCALCSIZE:
			{
				if (type != CUSTOM)
				{
					return DefWindowProc(handle(), message, wParam, lParam);
				}
				const int resizeBorderX = GetSystemMetrics(SM_CXFRAME);
				const int resizeBorderY = GetSystemMetrics(SM_CYFRAME);
				int padding = GetSystemMetrics(SM_CXPADDEDBORDER);
				NCCALCSIZE_PARAMS* params = (NCCALCSIZE_PARAMS*)lParam;
				RECT* reqClientRect = params->rgrc;

				reqClientRect->right -= resizeBorderX + padding - 1;
				reqClientRect->left += resizeBorderX + padding - 1;
				reqClientRect->bottom -= resizeBorderY + padding - 1;
				if (win32_window_is_maximized(handle()))
				{
					reqClientRect->top += padding;
				}
				else
				{
					reqClientRect->top += 1;
				}
				return WVR_ALIGNTOP | WVR_ALIGNLEFT;
			}
			case WM_NCHITTEST:
			{
				if (type != CUSTOM)
				{
					return DefWindowProc(handle(), message, wParam, lParam);
				}
				LRESULT hit = DefWindowProc(handle(), message, wParam, lParam);
				switch (hit) {
				case HTNOWHERE:
				case HTRIGHT:
				case HTLEFT:
				case HTTOPLEFT:
				case HTTOP:
				case HTTOPRIGHT:
				case HTBOTTOMRIGHT:
				case HTBOTTOM:
				case HTBOTTOMLEFT: {
					return hit;
				}
				}
				if (m_CloseButton)
				{
					return HTCLOSE;
				}
				if (m_MinButton)
				{
					return HTMINBUTTON;
				}
				if (m_MaxButton)
				{
					return HTMAXBUTTON;
				}
				if (m_titleBarHovered)
				{
					return HTCAPTION;
				}

				return HTCLIENT;
			}
			case WM_NCLBUTTONDOWN:
			{
				if (type != CUSTOM)
				{
					return DefWindowProc(handle(), message, wParam, lParam);
				}
				if (m_CloseButton || m_MaxButton || m_MinButton)
				{
					return 0;
				}
			return DefWindowProc(handle(), message, wParam, lParam);
			}
			case WM_NCLBUTTONUP:
			{
				if (type != CUSTOM)
				{
					return DefWindowProc(handle(), message, wParam, lParam);
				}
				if (m_CloseButton)
				{
					PostMessage(handle(), WM_CLOSE, 0, 0);
					return 0;
				}
				if (m_MinButton)
				{
					ShowWindow(handle(), SW_MINIMIZE);
				}
				if (m_MaxButton)
				{
					int mode = win32_window_is_maximized(handle()) ? SW_NORMAL : SW_MAXIMIZE;
					ShowWindow(handle(), mode);
				}
				return DefWindowProc(handle(), message, wParam, lParam);
			}
			case WM_NCLBUTTONDBLCLK:
			{
				if (type != CUSTOM) 
				{
					return DefWindowProc(handle(), message, wParam, lParam); 
				}
				int mode = win32_window_is_maximized(handle()) ? SW_NORMAL : SW_MAXIMIZE; 
				ShowWindow(handle(), mode); 
				return 0;
			}
			case WM_CLOSE:
				CUBE_CORE_INFO("Window was closed.");
				PostQuitMessage(0);
				return 0;
			}
		return DefWindowProc(hwnd, message, wParam, lParam);
	}

}
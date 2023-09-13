#pragma once
#include <string>
#include <Windows.h>
#include "Keyboard.h"
#include "Mouse.h"
#include "../exception/includes/CubeException.h"

namespace Cube
{
	enum WindowType : DWORD
	{
		FULL = WS_OVERLAPPEDWINDOW | WS_BORDER,
		STATIC = WS_OVERLAPPED,
		RESIZABLE = WS_SIZEBOX,
		POPUP = WS_POPUP
	};
	class Window
	{
	public:
		class Exception : public CubeException
		{
		public:
			Exception(int line, const char* file, HRESULT hResult);
			virtual const char* whatEx() const override;
			virtual const char* getType() const override;
			static std::string TranslateErrorCode(HRESULT hResult);
			HRESULT GetErrorCode() const;
			std::string GetErrorString() const;
		private:
			HRESULT hResult;
		};
	private:
		class WindowClass
		{
		public:
			static const wchar_t* getName();
			static HINSTANCE getInstance();
		private:
			WindowClass();
			~WindowClass();
			static constexpr const auto* wndClassName = L"Cube Engine";
			static WindowClass wndClass;
			HINSTANCE hInst;
		};
	public:
		void WindowShow();
		Window(int width, int height);
		~Window();
		HWND handle() { return hwnd; }
		void handle(HWND shwnd) { hwnd = shwnd; }
		static std::optional<int> ProcessMessages();
		Mouse mouse;
		Keyboard kbd;
	private:
		static LRESULT WINAPI HandleMessageSetup(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
		static LRESULT WINAPI HandleMessageThunk(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
		LRESULT HandleMessage(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
		int width;
		int height;
		HWND hwnd;
	};
}

#define CUBE_EXCEPTION(hResult) Window::Exception(__LINE__, __FILE__, hResult)
#define CUBE_LAST_EXCEPTION() Window::Exception(__LINE__, __FILE__, GetLastError())
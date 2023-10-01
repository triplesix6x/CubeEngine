#pragma once
#include <string>
#include <Windows.h>
#include <memory>
#include "Keyboard.h"
#include "Mouse.h"
#include "../render/includes/Graphics.h"
#include "../../exception/includes/CubeException.h"

namespace Cube
{
	enum WindowType : DWORD
	{
		FULL = WS_OVERLAPPEDWINDOW /*& ~WS_THICKFRAME & ~WS_MAXIMIZEBOX*/,
		STATIC = WS_OVERLAPPED,
		RESIZABLE = WS_SIZEBOX,
		POPUP = WS_POPUP
	};
	class Window
	{
	public:
		class Exception : public CubeException
		{
			using CubeException::CubeException;
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
		class NoGfxException : public Exception
		{
		public:
			using Exception::Exception;
			const char* getType() const;
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
		Window(int width, int height);
		~Window();
		HWND handle() { return hwnd; }
		void handle(HWND shwnd) { hwnd = shwnd; }
		static std::optional<MSG> ProcessMessages();
		int GetWidth() const noexcept;
		int GetHeight() const noexcept;
		Graphics& Gfx();
		Mouse mouse;
		Keyboard kbd;
		int wResize;
		int hResize;
	private:
		static LRESULT WINAPI HandleMessageSetup(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
		static LRESULT WINAPI HandleMessageThunk(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
		LRESULT HandleMessage(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
		int width;
		int height;
		HWND hwnd;
		std::unique_ptr<Graphics> pGfx;
	};
}

#define CUBE_EXCEPTION(hResult) Window::Exception(__LINE__, __FILE__, hResult)
#define HWND_NOGFX_EXCEPTION() Window::NoGfxException(__LINE__, __FILE__)
#define CUBE_LAST_EXCEPTION() Window::Exception(__LINE__, __FILE__, GetLastError())
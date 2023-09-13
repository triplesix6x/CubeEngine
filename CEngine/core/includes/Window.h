#pragma once
#include <string>
#include "Core.h"
#include <Windows.h>
#include "../exception/includes/CubeException.h"

namespace Cube
{
	class CUBE_API Window
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
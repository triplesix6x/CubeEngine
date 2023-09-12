#pragma once
#include <string>
#include "Core.h"
#include <Windows.h>

namespace Cube
{
	class CUBE_API Window
	{
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
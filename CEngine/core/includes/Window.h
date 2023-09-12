#pragma once
#include <string>
#include "Core.h"
#include <Windows.h>

namespace Cube
{
	const auto pClassName = L"Cube";
	class CUBE_API Window
	{
	public:
		Window(HINSTANCE hInstance);
		~Window();
		WNDCLASSEX wc;
		HWND hwnd;
		void WindowShow();
	};
}
#include "../includes/Application.h"
#include "../includes/Window.h"

namespace Cube
{

	Application::Application()
	{
		m_Window = new Window(1280, 720);
		m_Window->WindowShow();
	}

	Application::~Application()
	{

	}
	int Application::run()
	{
		while ( (gResult = GetMessage(&message, nullptr, 0, 0)) > 0)
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		};
		if (gResult == -1)
		{
			return -1;
		}
		else
		{
			return message.wParam;
		}
	}
}

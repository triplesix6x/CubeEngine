#include "../includes/Application.h"
#include "../includes/Window.h"

namespace Cube
{

	Application::Application() : m_Window(1280, 720)
	{
		m_Window.WindowShow();
	}

	Application::~Application()
	{

	}
	int Application::run()
	{
		while ( true )
		{
			if (const auto ecode = Window::ProcessMessages())
			{
				return *ecode;
			}
			doFrame();
		}
	}

	void Application::doFrame()
	{

	}
}

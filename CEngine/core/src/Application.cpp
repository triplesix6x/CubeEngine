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
		const float a = sin(timer.Peek()) / 0.5f + 0.5f;
		const float b = sin(timer.Peek()) / 1.0f + 0.5f;
		const float c = sin(timer.Peek()) / 2.0f + 0.5f;
		m_Window.Gfx().ClearBuffer(a, b, c);
		m_Window.Gfx().DrawTestTriangle();
		m_Window.Gfx().EndFrame();
	}
}

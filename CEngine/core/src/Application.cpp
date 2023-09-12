#include "../includes/Application.h"
#include "../includes/Window.h"

namespace Cube
{

	Application::Application(HINSTANCE hInstance)
	{
		m_Window = new Window(hInstance);
	}

	Application::~Application()
	{

	}
	void Application::run()
	{
		m_Window->WindowShow();
		while (true);
	}
}

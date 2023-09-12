#pragma once
#include "Core.h"
#include "Window.h"

namespace Cube
{

	class CUBE_API Application
	{
	public:
		Application(HINSTANCE hInstance);
		virtual ~Application();
		void run();
	private:
		Window* m_Window;
	};
	Application* CreateApplication(HINSTANCE hInstance);
	void ReleaseApplication(void* app);
}

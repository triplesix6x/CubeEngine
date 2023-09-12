#pragma once
#include "Core.h"
#include "Window.h"

namespace Cube
{

	class CUBE_API Application
	{
	public:
		Application();
		virtual ~Application();
		int run();
	private:
		Window* m_Window;
		MSG message;
		BOOL gResult;
	};
	Application* CreateApplication();
	void ReleaseApplication(void* app);
}

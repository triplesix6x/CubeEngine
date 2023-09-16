#pragma once
#include "Window.h"
#include "Timer.h"

namespace Cube
{

	class Application
	{
	public:
		Application();
		virtual ~Application();
		int run();
	private:
		void doFrame();
		Window m_Window;
		Timer timer;
		MSG message;
		BOOL gResult;
	};
}

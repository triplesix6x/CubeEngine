#pragma once
#include "Window.h"

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
		MSG message;
		BOOL gResult;
	};
}

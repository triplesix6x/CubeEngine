#pragma once
#include "Window.h"
#include "Timer.h"
#include "../render/includes/Box.h"

namespace Cube
{

	class Application
	{
	public:
		Application(int width, int height);
		virtual ~Application();
		int run();
	private:
		void doFrame();
		std::vector<std::unique_ptr<Drawable>> boxes;
		Window m_Window;
		Timer timer;
	};
}

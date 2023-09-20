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
		void doFrame();					//функция отрисовки одного кадра
		std::vector<std::unique_ptr<Drawable>> drawables;
		static constexpr size_t nDrawables = 50;
		Window m_Window;
		Timer timer;
	};
}

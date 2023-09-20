#pragma once
#include "Window.h"
#include "Timer.h"
#include "ImguiManager.h"
#include "../render/includes/Camera.h"
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
		ImguiManager imgui;
		float speed_factor = 1.0f;
		Camera cam;
		std::vector<std::unique_ptr<Drawable>> drawables;
		static constexpr size_t nDrawables = 200;
		Window m_Window;
		Timer timer;
	};
}

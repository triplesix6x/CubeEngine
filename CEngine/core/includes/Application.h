#pragma once
#include "Window.h"
#include "Timer.h"
#include "ImguiManager.h"
#include "../render/includes/Camera.h"
#include "../render/includes/PointLight.h"
#include "../render/includes/Mesh.h"
#include <set>

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
		Window m_Window;
		PointLight light;
		Model nano{ m_Window.Gfx(),"models\\nanosuit.obj" };
		Model suz{ m_Window.Gfx(),"models\\suzanne.obj" };
		Timer timer;
	};
}

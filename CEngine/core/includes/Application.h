#pragma once
#include "Window.h"
#include "Timer.h"
#include "ImguiManager.h"
#include "../render/includes/Camera.h"
#include "../render/includes/PointLight.h"
#include "../render/includes/Mesh.h"
#include "../render/includes/SkyBox.h"
#include <set>

namespace Cube
{

	class Application
	{
	public:
		Application(int width, int height);
		virtual ~Application();
		int run();
		void ShowSceneWindow();
	private:
		void doFrame();					//функция отрисовки одного кадра
		ImguiManager imgui;
		Camera cam;
		Window m_Window;
		PointLight light;
		std::vector<std::unique_ptr<Model>> models;
		SkyBox skybox;
		Timer timer;
	};
}

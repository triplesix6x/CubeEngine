#pragma once
#include "Window.h"
#include "Timer.h"
#include "ImguiManager.h"
#include "Log.h"
#include "../render/includes/Camera.h"
#include "../render/includes/PointLight.h"
#include "../render/includes/Mesh.h"
#include "../render/includes/TestPlane.h"
#include "../render/includes/SkyBox.h"
#include <set>

namespace Cube
{

	class Application
	{
		friend class SceneSerializer;
	public:
		Application(int width, int height, WindowType type);
		virtual ~Application();
		int run();
		void ShowSceneWindow();
		void ShowMenuBar();

		void AddObj();
		void AddCube();

		void ShowToolBar();
		void HadleInput(float dt);

		void showLightHelp();

		void showSettingsWindow();

	private:
		bool lHelpWindowOpen = false;
		bool projSettWindowOpen = false;
		void doFrame();					//функция отрисовки одного кадра
		ImguiManager imgui;
		Window m_Window;
		Timer timer;
		Camera cam;
		Lights light;
		std::vector<std::unique_ptr<Model>> models;
		ID3D11ShaderResourceView* pCubeIco = nullptr;
		std::unique_ptr<SkyBox> skybox;
		Model* pSelectedModel;
		bool drawGrid = true;
		std::chrono::milliseconds maxfps = std::chrono::milliseconds(14);
		bool nofpslimit = true;
		std::filesystem::path scenePath = "Unnamed Scene";
		int id = 0;
	};
}

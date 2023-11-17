#pragma once
#include "Window.h"
#include "Timer.h"
#include "ImguiManager.h"
#include "Log.h"
#include "../render/includes/Camera.h"
#include "../render/includes/PointLight.h"
#include "../render/includes/Mesh.h"
#include "../render/includes/SkyBox.h"
#include <set>

namespace Cube
{

	class Application
	{
		friend class SceneSerializer;
	public:
		Application(int width, int height, WindowType type);
		~Application();

		//Функция игрового цикла
		int run();

		//Функция добавления объекта из файла
		void AddObj();

		void AddCube();

		//Обработка клавиатуры и мыши
		void HadleInput(float dt);

		//Функции интерфейса ImGui
		void ShowSceneWindow();
		void ShowMenuBar();
		void showLightHelp();
		void showSettingsWindow();
		void ShowToolBar();

		//Функции сериализации и десериализации сцены
		void newScene();
		void openScene();
		void saveScene();
		void saveSceneAs();

	private:
		bool lHelpWindowOpen = false;
		bool projSettWindowOpen = false;
		bool drawGrid = true;
		bool nofpslimit = true;

		//функция отрисовки кадра
		void doFrame();					

		ImguiManager imgui;
		Window m_Window;
		Timer timer;
		Camera cam;
		Lights light;
		std::vector<std::unique_ptr<Model>> models;
		ID3D11ShaderResourceView* pCubeIco = nullptr;
		std::unique_ptr<SkyBox> skybox;
		Model* pSelectedModel;
		std::chrono::milliseconds maxfps = std::chrono::milliseconds(14);
		std::filesystem::path scenePath = "Unnamed Scene";

		int id = 0;
	};
}

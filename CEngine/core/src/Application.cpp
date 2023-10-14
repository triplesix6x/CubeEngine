#include "../includes/Application.h"
#include "../render/includes/CVertex.h"
#include "../includes/Window.h"
#include "../../imgui/imgui.h"
#include "../../imgui/imgui_impl_win32.h"
#include "../../imgui/imgui_impl_dx11.h"
#include "../../imgui/imgui_internal.h"
#include "../includes/CMath.h"
#include "../includes/ImguiThemes.h"
#include <Commdlg.h>
#include <memory>
#include <stdio.h>
#include <string.h>
#include <chrono>


namespace Cube
{
	Application::Application(int width, int height, WindowType type) 
		: 
		m_Window(width, height, type),
		light(m_Window.Gfx()), 
		skybox(std::make_unique<SkyBox>(m_Window.Gfx(), L"textures\\skyboxmain.dds"))
	{
		models.push_back(std::make_unique<Model>(m_Window.Gfx(), "models\\cube.obj", id, "Cube"));
		++id;

		m_Window.Gfx().SetTexture(&pCubeIco, L"icons\\cubeico2.png");
	}

	Application::~Application()
	{
		pCubeIco->Release();
		models.clear();
	}

	void Application::HadleInput(float dt)
	{
		const auto io = ImGui::GetIO();
		if (!io.WantCaptureMouse)
		{
			if (m_Window.kbd.KeyIsPressed('W'))
			{
				cam.Translate({ 0.0f, 0.0f, dt});
			}
			if (m_Window.kbd.KeyIsPressed('S'))
			{
				cam.Translate({ 0.0f, 0.0f, -dt});
			}
			if (m_Window.kbd.KeyIsPressed('A'))
			{
				cam.Translate({ -dt, 0.0f, 0.0f });
			}
			if (m_Window.kbd.KeyIsPressed('D'))
			{
				cam.Translate({ dt, 0.0f, 0.0f });
			}
			if (m_Window.kbd.KeyIsPressed(VK_SPACE))
			{
				cam.Translate({ 0.0f, dt, 0.0f });
			}
			if (m_Window.kbd.KeyIsPressed(VK_SHIFT))
			{
				cam.Translate({ 0.0f, -dt, 0.0f });
			}
			while (const auto d = m_Window.mouse.ReadRawDelta())
			{
				if (m_Window.mouse.RightIsPressed())
				{
					cam.Rotate((float)d->x, (float)d->y);
				}
			}
			while (const auto b = m_Window.mouse.Read())
			{
				if (b->GetType() == Mouse::Event::Type::WheelUp)
				{
					cam.Translate({ 0.0f, 0.0f, dt * 10});
				}
				if (b->GetType() == Mouse::Event::Type::WheelDown)
				{
					cam.Translate({ 0.0f, 0.0f, -dt * 10});
				}
			}
		}
	}

	int Application::run()
	{
		while ( true )
		{
			if (const auto ecode = Window::ProcessMessages())
			{
				return ecode->wParam;
			}
			auto dt = timer.Mark();
			
			HadleInput(dt);
			doFrame();
		}
	}

	void Application::doFrame()
	{


		m_Window.Gfx().ClearBuffer(0.07f, 0.07f, 0.07f);		//Очистка текущего буфера свап чейна
		m_Window.Gfx().SetCamera(cam.GetMatrix());

		ImGuiID did = m_Window.Gfx().ShowDocksape();
		ImGuiDockNode* node = ImGui::DockBuilderGetCentralNode(did);
		m_Window.Gfx().CreateViewport(node->Size.x, node->Size.y, node->Pos.x, node->Pos.y);
		m_Window.Gfx().SetProjection(DirectX::XMMatrixPerspectiveFovLH(to_rad(75.0f), node->Size.x / node->Size.y, 0.01f, 150.0f));

		ShowMenuBar();

		if (skybox)
		{
			skybox->Draw(m_Window.Gfx());
		}

		light.Bind(m_Window.Gfx(), cam.GetMatrix());
	

		for (auto& m : models) 
		{
			m->Draw(m_Window.Gfx());
		}



		light.drawSpheres(m_Window.Gfx());
		


		if (drawGrid)
		{
			m_Window.Gfx().DrawGrid(cam.pos);
		}


		ShowSceneWindow();
		ShowToolBar();
		showLightHelp();

		m_Window.Gfx().EndFrame();							//Замена буфера свап чейна

	}


	void Application::ShowSceneWindow()
	{
		const auto io = ImGui::GetIO();
		ImGui::SetNextWindowSize(ImGui::GetContentRegionAvail(), ImGuiCond_FirstUseEver);

		ImGui::Begin("Scene");

		ImGuiID dockspace_id = ImGui::GetID("DockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_NoTabBar);
		static bool first_time = true;
		if (first_time) {
			first_time = false;
			ImGui::DockBuilderRemoveNode(dockspace_id);
			ImGui::DockBuilderAddNode(dockspace_id);
			ImGui::DockBuilderSetNodePos(dockspace_id, ImGui::GetWindowPos());
			ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetWindowSize());

			ImGuiID dock_id_up = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Up, 0.8f, nullptr, &dockspace_id);
			ImGuiID dock_id_down = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 0.2f, nullptr, &dockspace_id);
			ImGui::DockBuilderDockWindow("Objects", dock_id_up);
			ImGui::DockBuilderDockWindow("Proprieties", dock_id_down);

			ImGui::DockBuilderFinish(dockspace_id);
		}
		ImGui::Begin("Objects");
		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::MenuItem("Add object from file..."))
			{
				AddObj();
			}
			if (ImGui::MenuItem("Add Light"))
			{
				light.AddLight(m_Window.Gfx());
			}
			if (models.size() != 0)
			{
				for (int i = 0; i < models.size(); ++i)
				{
					std::string name = "Delete " + models[i]->modelName;
					if (ImGui::MenuItem(name.c_str()))
					{
						models.erase(models.begin() + i);
					}
				}
			}
			for (int i = 0; i < light.getLighstCount(); ++i)
			{
				std::string name = "Delete " + light.getName(i);
				if (ImGui::MenuItem(name.c_str()))
				{
					light.DeleteLight(i);
				}
			}
			if (ImGui::MenuItem("Clear Scene"))
			{
				models.clear();
				light.clearLights();
			}
			ImGui::EndPopup();
		}
		for (int i = 0; i < models.size(); ++i)
		{
			const int selectedId = (pSelectedModel == nullptr) ? -1 : pSelectedModel->GetId();
			bool expanded = ImGui::TreeNodeEx((void*)(intptr_t)models[i]->GetId(), 0, models[i]->modelName.c_str());
			if (ImGui::IsItemClicked())
			{
				pSelectedModel = models[i].get();
			}
			if (expanded)
			{
				models[i]->ShowWindow(m_Window.Gfx(), pSelectedModel);
				ImGui::TreePop();
				ImGui::Spacing();
			}
			if (ImGui::IsMouseDoubleClicked(0) && ImGui::IsItemClicked())
			{
				pSelectedModel = nullptr;
			}
		}
		
		light.spawnWnds();

		cam.SpawnControlWindow();

		auto th = ImGui::GetWindowHeight();
		std::string text = "Application average %.3f ms/frame (%.1f FPS)";
		ImGui::SetCursorPosY(th - ImGui::CalcTextSize(text.c_str()).y - 10.0f);
		ImGui::Text(text.c_str(), 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		ImGui::End();

		ImGui::End();
	}



	void Application::ShowMenuBar()
	{ 
		if (ImGui::BeginMainMenuBar())
		{
			if (m_Window.type == CUSTOM)
			{
				if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered())
				{
					m_Window.m_titleBarHovered = true;
				}
				else
				{
					m_Window.m_titleBarHovered = false;
				}
				auto tw = ImGui::GetWindowWidth();
				std::string text = "Cube Engine";
				ImGui::SetCursorPosX((tw - ImGui::CalcTextSize(text.c_str()).x) / 2);
				ImGui::Text(text.c_str());

				std::string close_text = "X";
				ImGui::SetCursorPosX((tw - ImGui::CalcTextSize(close_text.c_str()).x) - 10.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
				ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_MenuBarBg]);
				ImGui::Button(close_text.c_str(), ImVec2{ 20.0f, 20.0f });
				if (ImGui::IsItemHovered())
				{
					m_Window.m_CloseButton = true;
				}
				else
				{
					m_Window.m_CloseButton = false;
				}
				ImGui::PopStyleVar(2);
				ImGui::PopStyleColor();

				char max_sym = '=';
				std::string max_text{max_sym};
				ImGui::SetCursorPosX(tw - ImGui::CalcTextSize(max_text.c_str()).x - 32.5f);
				ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
				ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_MenuBarBg]);
				ImGui::Button(max_text.c_str(), ImVec2{ 20.0f, 20.0f });
				if (ImGui::IsItemHovered())
				{
					m_Window.m_MaxButton = true;
				}
				else
				{
					m_Window.m_MaxButton = false;
				}
				ImGui::PopStyleVar(2);
				ImGui::PopStyleColor();

				std::string min_text = "-";
				ImGui::SetCursorPosX(tw - ImGui::CalcTextSize(min_text.c_str()).x - 57.5f);
				ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
				ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_MenuBarBg]);
				ImGui::Button(min_text.c_str(), ImVec2{ 20.0f, 20.0f });
				if (ImGui::IsItemHovered())
				{
					m_Window.m_MinButton = true;
				}
				else
				{
					m_Window.m_MinButton = false;
				}
				ImGui::PopStyleVar(2);
				ImGui::PopStyleColor();
			}
			ImGui::SetCursorPosX(10.0f);
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("Create"))
				{
				}
				if (ImGui::MenuItem("Open"))
				{
				}
				if (ImGui::MenuItem("Save"))
				{
				}
				if (ImGui::MenuItem("Save as..")) {
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Scene")) {
				if (ImGui::MenuItem("Add object from file..."))
				{
					AddObj();
				}
				if (ImGui::MenuItem("Add Light"))
				{
					light.AddLight(m_Window.Gfx());
				}
				if (models.size() != 0)
				{
					for (int i = 0; i < models.size(); ++i)
					{
						std::string name = "Delete " + models[i]->modelName;
						if (ImGui::MenuItem(name.c_str()))
						{
							models.erase(models.begin() + i);
						}
					}
				}
				for (int i = 0; i < light.getLighstCount(); ++i)
				{
					std::string name = "Delete " + light.getName(i);
					if (ImGui::MenuItem(name.c_str()))
					{
						light.DeleteLight(i);
					}
				}
				if (ImGui::MenuItem("Clear Scene"))
				{
					models.clear();
					light.clearLights();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Themes")) {
				if (ImGui::MenuItem("Default Dark"))
				{
					SetDDTheme();
				}
				if (ImGui::MenuItem("Classic Light"))
				{
					SetLightTheme();
				}
				if (ImGui::MenuItem("Deep Purple"))
				{
					SetDPTheme();
				}
				if (ImGui::MenuItem("Sailor moon"))
				{
					SetSMTheme();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Skybox")) {
				if (ImGui::MenuItem("Set Skybox 1"))
				{
					skybox.release();
					skybox = std::make_unique<SkyBox>(m_Window.Gfx(), L"textures\\skyboxmain.dds");
				}
				if (ImGui::MenuItem("Set Skybox 2"))
				{
					skybox.release();
					skybox = std::make_unique<SkyBox>(m_Window.Gfx(), L"textures\\skybox2.dds");
				}
				if (ImGui::MenuItem("Set Skybox 3"))
				{
					skybox.release();
					skybox = std::make_unique<SkyBox>(m_Window.Gfx(), L"textures\\earth.dds");
				}
				if (ImGui::MenuItem("Delete Skybox"))
				{
					skybox.release();
				}
				if (ImGui::MenuItem("Load Skybox from file..."))
				{
					wchar_t fname[260];
					OPENFILENAME ofn;
					ZeroMemory(&ofn, sizeof(ofn));
					ofn.lStructSize = sizeof(ofn);
					ofn.hwndOwner = NULL;
					ofn.lpstrFile = fname;
					ofn.lpstrFile[0] = '\0';
					ofn.nMaxFile = sizeof(fname);
					ofn.lpstrFilter = L"DDS files(*.dds)\0*.dds\0\0";
					ofn.lpstrFileTitle = NULL;
					ofn.nFilterIndex = 1;
					ofn.nMaxFileTitle = 0;
					ofn.lpstrInitialDir = L"";
					ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_EXPLORER | OFN_NOCHANGEDIR;
					if (GetOpenFileName(&ofn))
					{
						std::wstring name(fname);
						auto x = wcstok(fname, L".");
						x = wcstok(NULL, L".");
						if (wcscmp(x, L"dds") == 0)
						{
							skybox.release();
							skybox = std::make_unique<SkyBox>(m_Window.Gfx(), name.c_str());
						}
						else
						{
							MessageBox(nullptr, L"You can only load .dds and files for skybox", L"Loading error", MB_OK | MB_ICONEXCLAMATION);
						}
					}
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Help"))
			{
				if (ImGui::MenuItem("Light Help"))
				{
					lopen = true;
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
	}

	void Application::showLightHelp()
	{
		if (lopen)
		{
			ImGuiIO& io = ImGui::GetIO();
			ImGui::SetNextWindowSize(ImVec2{ 400, 700 }, ImGuiCond_Appearing);
			ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
			ImGui::Begin("Light Help", &lopen, ImGuiWindowFlags_NoCollapse);
			ImGui::SeparatorText("POINT LIGHT VALUES:");
			ImGui::Columns(1);
			ImGui::TextWrapped("Cube Engine uses one type of shading - Phong shading. So here are presented the recommended values for Point light sources, depending on them ranges from objects:");

			ImGui::Columns(4);
			ImGui::BulletText("Range ->");
			ImGui::BulletText("0.7 ->");
			ImGui::BulletText("2 ->");
			ImGui::BulletText("5 ->");
			ImGui::BulletText("10 ->");
			ImGui::BulletText("16 ->");
			ImGui::BulletText("20 ->");
			ImGui::BulletText("32.5 ->");
			ImGui::BulletText("60 ->");
			ImGui::BulletText("325 ->");

			ImGui::NextColumn();
			ImGui::Text("Constant");
			ImGui::Text("1.0");
			ImGui::Text("1.0");
			ImGui::Text("1.0");
			ImGui::Text("1.0");
			ImGui::Text("1.0");
			ImGui::Text("1.0");
			ImGui::Text("1.0");
			ImGui::Text("1.0");
			ImGui::Text("1.0");

			ImGui::NextColumn();
			ImGui::Text("Linear");
			ImGui::Text("0.7");
			ImGui::Text("0.22");
			ImGui::Text("0.09");
			ImGui::Text("0.045");
			ImGui::Text("0.027");
			ImGui::Text("0.022");
			ImGui::Text("0.014");
			ImGui::Text("0.007");
			ImGui::Text("0.0014");

			ImGui::NextColumn();
			ImGui::Text("Quadratic");
			ImGui::Text("1.8");
			ImGui::Text("0.20");
			ImGui::Text("0.032");
			ImGui::Text("0.0075");
			ImGui::Text("0.0028");
			ImGui::Text("0.0019");
			ImGui::Text("0.0007");
			ImGui::Text("0.0002");
			ImGui::Text("0.000007");
			ImGui::Columns(1);
			ImGui::End();
		}
	}

	void Application::ShowToolBar()
	{
		const auto io = ImGui::GetIO();
		ImGui::SetNextWindowSize({ 150, ImGui::GetContentRegionAvail().y }, ImGuiCond_FirstUseEver);
		ImGui::Begin("ToolBar");

		AddCube();
		ImGui::Checkbox("Draw Grid", &drawGrid);
		ImGui::End();
	}

	void Application::AddObj()
	{
		char cname[260];
		OPENFILENAMEA ofn;
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = NULL;
		ofn.lpstrFile = cname;
		ofn.lpstrFile[0] = '\0';
		ofn.nMaxFile = sizeof(cname);
		ofn.lpstrFilter = "OBJ files(*.obj)\0*.obj\0GLTF files(*.gltf)\0*.gltf\0MD5MESH files(*.md5mesh)\0*.md5mesh\0\0";
		ofn.lpstrFileTitle = NULL;
		ofn.nFilterIndex = 1;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = "";
		ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_EXPLORER | OFN_NOCHANGEDIR;
		if (GetOpenFileNameA(&ofn))
		{
			std::string nname(cname);
			auto x = strtok(cname, ".");
			x = strtok(NULL, ".");
			if (strcmp(x, "obj") == 0 or strcmp(x, "gltf")or strcmp(x, "md5mesh") == 0)
			{
					models.push_back(std::make_unique<Model>(m_Window.Gfx(), nname, id));
					++id;
			}
			else
			{
				MessageBox(nullptr, L"You can only load .obj and .gltf files", L"Loading error", MB_OK | MB_ICONEXCLAMATION);
			}
		}
	}

	void Application::AddCube()
	{

		ImGui::Image((void*)pCubeIco, ImVec2{ 24.0f, 24.0f });
		ImGui::SameLine();
		if (ImGui::Button("Add Cube"))
		{
			models.push_back(std::make_unique<Model>(m_Window.Gfx(), "models\\cube.obj", id, "Cube"));
			++id;
		}
	}
}

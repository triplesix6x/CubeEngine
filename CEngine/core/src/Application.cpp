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



namespace Cube
{
	Application::Application(int width, int height) : m_Window(width, height), light(m_Window.Gfx()), skybox(m_Window.Gfx(), L"textures\\skyboxmain.dds")
	{
		models.push_back(std::make_unique<Model>(m_Window.Gfx(), "models\\cube.obj"));
	}

	Application::~Application()
	{

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
		m_Window.Gfx().SetProjection(DirectX::XMMatrixPerspectiveFovLH(to_rad(75.0f), node->Size.x / node->Size.y, 0.5f, 150.0f));

		ShowMenuBar();
		skybox.Draw(m_Window.Gfx());
		light.Bind(m_Window.Gfx(), cam.GetMatrix());
	

		for (auto& m : models) 
		{
			m->Draw(m_Window.Gfx());
		}
		
		light.Draw(m_Window.Gfx());

		m_Window.Gfx().DrawGrid(cam.pos);

		ShowSceneWindow();
		ShowToolBar();
		m_Window.Gfx().EndFrame();							//Замена буфера свап чейна

	}


	void Application::ShowSceneWindow()
	{
		const auto io = ImGui::GetIO();
		ImGui::SetNextWindowSize(ImGui::GetContentRegionAvail(), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2{io.DisplaySize.x - 500, io.DisplaySize.y/2}, ImGuiCond_FirstUseEver);

		ImGui::Begin("Scene");
		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::MenuItem("Add object from file..."))
			{
				AddObj();
			}
			if (models.size() != 0)
			{
				for (int i = 0; i < models.size(); ++i)
				{
					std::string name = "Delete Object " + std::to_string(i);
					if (ImGui::MenuItem(name.c_str()))
					{
						models.erase(models.begin() + i);
					}
				}
			}
			ImGui::EndPopup();
		}
		for (int i = 0; i < models.size(); ++i)
		{
			std::string name = "Object " + std::to_string(i);
			models[i]->ShowWindow(name.c_str());
		}

		light.SpawnControlWindow();

		cam.SpawnControlWindow();

		auto th = ImGui::GetWindowHeight();
		std::string text = "Application average %.3f ms/frame (%.1f FPS)";
		ImGui::SetCursorPosY(th - ImGui::CalcTextSize(text.c_str()).y - 10.0f);
		ImGui::Text(text.c_str(), 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		ImGui::End();
	}

	void Application::ShowMenuBar()
	{
		if (ImGui::BeginMainMenuBar())
		{
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
			if (ImGui::BeginMenu("Scene")) {
				if (ImGui::MenuItem("Add object from file..."))
				{
					AddObj();
				}
				if (models.size() != 0)
				{
					for (int i = 0; i < models.size(); ++i)
					{
						std::string name = "Delete Object " + std::to_string(i);
						if (ImGui::MenuItem(name.c_str()))
						{
							models.erase(models.begin() + i);
						}
					}
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
	}

	void Application::ShowToolBar()
	{
		const auto io = ImGui::GetIO();
		ImGui::SetNextWindowSize({ 150, ImGui::GetContentRegionAvail().y }, ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2{ 100, io.DisplaySize.y / 2 }, ImGuiCond_FirstUseEver);
		ImGui::Begin("ToolBar");


		AddCube();

		ImGui::End();
	}

	void Application::AddObj()
	{
		wchar_t fname[260];
		OPENFILENAME ofn;
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = NULL;
		ofn.lpstrFile = fname;
		ofn.lpstrFile[0] = '\0';
		ofn.nMaxFile = sizeof(fname);
		ofn.lpstrFilter = L"OBJ files(*.obj)\0*.obj\0GLTF files(*.gltf)\0*.gltf\0\0";
		ofn.lpstrFileTitle = NULL;
		ofn.nFilterIndex = 1;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = L"";
		ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_EXPLORER;
		if (GetOpenFileName(&ofn))
		{
			char cname[260];
			WideCharToMultiByte(CP_UTF8, 0, fname, -1, cname, 260, NULL, NULL);
			std::string nname(cname);
			auto x = strtok(cname, ".");
			x = strtok(NULL, ".");
			if (strcmp(x, "obj") == 0 or strcmp(x, "gltf") == 0)
			{
				models.push_back(std::make_unique<Model>(m_Window.Gfx(), nname));
			}
			else
			{
				MessageBox(nullptr, L"You can only load .obj and .gltf files", L"Loading error", MB_OK | MB_ICONEXCLAMATION);
			}
		}
	}

	void Application::AddCube()
	{
		wchar_t path[260];
		GetModuleFileName(NULL, path, 260);
		int len = wcslen(path);
		for (int i = 1; i < 15; ++i)
		{
			path[len - i] = '\0';
		}

		char bpath[260];
		GetModuleFileNameA(NULL, bpath, 260);
		int blen = strlen(bpath);
		for (int i = 1; i < 15; ++i)
		{
			bpath[blen - i] = '\0';
		}

		ID3D11ShaderResourceView* pTextureView;
		m_Window.Gfx().SetTexture(&pTextureView, wcscat(path, L"icons\\cubeico2.png"));
		ImGui::Image((void*)pTextureView, ImVec2{ 24.0f, 24.0f });
		ImGui::SameLine();
		if (ImGui::Button("Add Cube"))
			models.push_back(std::make_unique<Model>(m_Window.Gfx(), strcat(bpath, "models\\cube.obj")));
	}
}

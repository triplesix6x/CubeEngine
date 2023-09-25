#include "../includes/Application.h"
#include "../render/includes/CVertex.h"
#include "../includes/Window.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_win32.h"
#include "../imgui/imgui_impl_dx11.h"
#include "../imgui/imgui_internal.h"
#include <Commdlg.h>
#include <memory>
#include <stdio.h>
#include <string.h>



namespace Cube
{
	Application::Application(int width, int height) : m_Window(width, height), light(m_Window.Gfx())
	{
	}

	Application::~Application()
	{

	}
	int Application::run()
	{
		while ( true )
		{
			if (const auto ecode = Window::ProcessMessages())
			{
				return ecode->wParam;
			}
			doFrame();
		}
	}

	void Application::doFrame()
	{
		auto dt = timer.Mark() * speed_factor;
		if (m_Window.wResize != 0 && m_Window.hResize != 0)
		{
			m_Window.Gfx().Resize(m_Window.wResize, m_Window.hResize);
			m_Window.hResize = m_Window.wResize = 0;
		}

		m_Window.Gfx().ClearBuffer(0.07f, 0.07f, 0.07f);		//Очистка текущего буфера свап чейна
		m_Window.Gfx().SetCamera(cam.GetMatrix());

		ImGuiID did = m_Window.Gfx().ShowDocksape();
		ImGuiDockNode* node = ImGui::DockBuilderGetCentralNode(did);
		m_Window.Gfx().CreateViewport(node->Size.x, node->Size.y, node->Pos.x, node->Pos.y);
		m_Window.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, node->Size.y / node->Size.x, 0.5f, 100.0f));
		m_Window.Gfx().ShowMenuBar();

		light.Bind(m_Window.Gfx(), cam.GetMatrix());


		for (auto& m : models)
		{
			m->Draw(m_Window.Gfx());
		}

		light.Draw(m_Window.Gfx());

		ShowSceneWindow();

		m_Window.Gfx().EndFrame();							//Замена буфера свап чейна
	}



	void Application::ShowSceneWindow()
	{
		ImGui::Begin("Scene");
		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::MenuItem("New object"))
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
				ofn.lpstrInitialDir = L"models";
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
			if (models.size() != 0)
			{
				for (int i = 0; i < models.size(); ++i)
				{
					std::string name = "Delete Model " + std::to_string(i);
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
			std::string name = "Model " + std::to_string(i);
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

}

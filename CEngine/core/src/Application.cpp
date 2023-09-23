#include "../includes/Application.h"
#include "../render/includes/Box.h"
#include "../render/includes/SkinnedBox.h"
#include "../render/includes/Cylinder.h"
#include "../render/includes/Pyramid.h"
#include "../render/includes/AssTest.h"
#include "../includes/Window.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_win32.h"
#include "../imgui/imgui_impl_dx11.h"
#include "../imgui/imgui_internal.h"
#include <memory>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


namespace Cube
{

	Application::Application(int width, int height) : m_Window(width, height), light(m_Window.Gfx())
	{
		std::mt19937 rng(std::random_device{}());
		std::uniform_real_distribution<float> adist(0.0f, 3.1415f * 2.0f);
		std::uniform_real_distribution<float> ddist(0.0f, 3.1415f * 0.5f);
		std::uniform_real_distribution<float> odist(0.0f, 3.1415f * 0.08f);
		std::uniform_real_distribution<float> rdist(6.0f, 20.0f);
		std::uniform_real_distribution<float> bdist{ 0.4f,3.0f };
		std::uniform_real_distribution<float> cdist{ 0.0f,1.0f };
		std::uniform_int_distribution<int> tdist{ 3,30 };
		for (auto i = 0; i < nDrawables; ++i)
		{
			const DirectX::XMFLOAT3 mat = { cdist(rng),cdist(rng),cdist(rng) };
			drawables.push_back(std::make_unique<Box>(
				m_Window.Gfx(), rng, adist, ddist,
				odist, rdist, bdist, mat));
			drawables.push_back(std::make_unique<Cylinder>(
				m_Window.Gfx(), rng, adist, ddist, odist,
				rdist, bdist, tdist));
			drawables.push_back(std::make_unique<Pyramid>(
				m_Window.Gfx(), rng, adist, ddist, odist,
				rdist, tdist));
			drawables.push_back(std::make_unique<SkinnedBox>(
				m_Window.Gfx(), rng, adist, ddist, odist,
				rdist));
			drawables.push_back(std::make_unique<AssTest>(
				m_Window.Gfx(), rng, adist, ddist, odist,
				rdist, mat, 1.5f));
		}
		for (auto& pd : drawables)
		{
			if (auto pb = dynamic_cast<Box*>(pd.get()))
			{
				boxes.push_back(pb);
			}
		}
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
		m_Window.Gfx().ClearBuffer(0.07f, 0.0f, 0.12f, m_Window.GetWidth(), m_Window.GetHeight());		//Очистка текущего буфера свап чейна
		m_Window.Gfx().SetCamera(cam.GetMatrix());
		light.Bind(m_Window.Gfx(), cam.GetMatrix());
		for (auto& b : drawables)
		{
			b->Update(dt);
			b->Draw(m_Window.Gfx());
		}
		light.Draw(m_Window.Gfx());
		ImGuiID did = m_Window.Gfx().ShowDocksape();
		ImGuiDockNode* node = ImGui::DockBuilderGetCentralNode(did);
		m_Window.Gfx().CreateViewport(node->Size.x, node->Size.y, node->Pos.x, node->Pos.y);
		m_Window.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, node->Size.y / node->Size.x, 0.5f, 100.0f));

		m_Window.Gfx().ShowMenuBar();
		ImGui::ShowMetricsWindow();
		SpawnSimulationWindow();
		SpawnBoxWindowManagerWindow();
		SpawnBoxWindows();
		cam.SpawnControlWindow();
		light.SpawnControlWindow();
		

		m_Window.Gfx().EndFrame(m_Window.GetWidth(), m_Window.GetHeight());							//Замена буфера свап чейна
	}




	void Application::SpawnSimulationWindow()
	{
		if (ImGui::Begin("Simulation Speed"))
		{
			ImGui::SliderFloat("Speed Factor", &speed_factor, 0.0f, 10.0f);
			if (ImGui::Button("Reset"))
			{
				speed_factor = 1.0f;
			}
		}
		ImGui::End();
	}
	void Application::SpawnBoxWindowManagerWindow()
	{
		if (ImGui::Begin("Boxes"))
		{
			using namespace std::string_literals;
			const auto preview = comboBoxIndex ? std::to_string(*comboBoxIndex) : "Choose a box..."s;
			if (ImGui::BeginCombo("Box Number", preview.c_str()))
			{
				for (int i = 0; i < boxes.size(); i++)
				{
					const bool selected = true;
					if (ImGui::Selectable(std::to_string(i).c_str(), selected))
					{
						comboBoxIndex = i;
					}
					if (selected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
			if (ImGui::Button("Spawn Control Window") && comboBoxIndex)
			{
				boxControlIDs.insert(*comboBoxIndex);
				comboBoxIndex.reset();
			}
		}
		ImGui::End();
	}
	void Application::SpawnBoxWindows()
	{
		for (auto i = boxControlIDs.begin(); i != boxControlIDs.end();)
		{
			if (!boxes[*i]->SpawnControlWindow(*i, m_Window.Gfx()))
			{
				i = boxControlIDs.erase(i);
			}
			else
			{
				i++;
			}
		}
	}
}

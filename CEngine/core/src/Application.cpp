#include "../includes/Application.h"
#include "../render/includes/Box.h"
#include "../render/includes/Melon.h"
#include "../render/includes/Pyramid.h"
#include "../render/includes/Sheet.h"
#include "../render/includes/SkinnedBox.h"
#include "../includes/Window.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_win32.h"
#include "../imgui/imgui_impl_dx11.h"
#include "../imgui/imgui_internal.h"
#include <memory>


namespace Cube
{

	Application::Application(int width, int height) : m_Window(width, height)
	{
		m_Window.WindowShow();
		std::mt19937 rng(std::random_device{}());
		std::uniform_real_distribution<float> adist(0.0f, 3.1415f * 2.0f);
		std::uniform_real_distribution<float> ddist(0.0f, 3.1415f * 0.5f);
		std::uniform_real_distribution<float> odist(0.0f, 3.1415f * 0.08f);
		std::uniform_real_distribution<float> rdist(6.0f, 20.0f);
		std::uniform_real_distribution<float> bdist{ 0.4f,3.0f };
		std::uniform_int_distribution<int> latdist{ 5,20 };
		std::uniform_int_distribution<int> longdist{ 10,40 };
		std::uniform_int_distribution<int> typedist{ 0,2 };
		for (auto i = 0; i < nDrawables; ++i)
		{
			drawables.push_back(std::make_unique<SkinnedBox>(
				m_Window.Gfx(), rng, adist, ddist,
				odist, rdist));
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
		m_Window.Gfx().ClearBuffer(0.07f, 0.0f, 0.12f, m_Window.GetWidth(), m_Window.GetHeight());		//Очистка текущего буфера свап чейна
		m_Window.Gfx().SetCamera(cam.GetMatrix());
		for (auto& b : drawables)
		{
			b->Update(dt);
			b->Draw(m_Window.Gfx());
		}
		ImGuiID did = m_Window.Gfx().ShowDocksape();
		ImGuiDockNode* node = ImGui::DockBuilderGetCentralNode(did);
		m_Window.Gfx().CreateViewport(node->Size.x, node->Size.y, node->Pos.x - 8, node->Pos.y - 31);
		m_Window.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, node->Size.y / node->Size.x, 0.5f, 100.0f));
		if (ImGui::Begin("Simulation Speed"))
		{
			ImGui::SliderFloat("Speed Factor", &speed_factor, 0.0f, 10.0f);
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			if (ImGui::Button("Reset"))
			{
				speed_factor = 1.0f;
			}
		}
		ImGui::End();
		cam.SpawnControlWindow();
		m_Window.Gfx().EndFrame(m_Window.GetWidth(), m_Window.GetHeight());							//Замена буфера свап чейна
	}
}

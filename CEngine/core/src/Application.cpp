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
		for (auto i = 0; i < nDrawables; i++)
		{
			drawables.push_back(std::make_unique<SkinnedBox>(
				m_Window.Gfx(), rng, adist, ddist,
				odist, rdist));
		}
		m_Window.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, static_cast<float>(height) / static_cast<float>(width), 0.5f, 40.0f));
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
		m_Window.Gfx().ClearBuffer(0.07f, 0.0f, 0.12f);		//Очистка текущего буфера свап чейна
		m_Window.Gfx().SetCamera(cam.GetMatrix());
		if (m_Window.kbd.KeyIsPressed(VK_SPACE))
		{
			m_Window.Gfx().CreateTestViewport();
		}
		else
		{
			m_Window.Gfx().CreateViewport(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
		}
		for (auto& b : drawables)
		{
			b->Update(dt);
			b->Draw(m_Window.Gfx());
		}
		ImGui::ShowDemoWindow();
		if (ImGui::Begin("Simulation Speed", NULL, ImGuiWindowFlags_NoResize))
		{
			ImGui::SliderFloat("Speed Factor", &speed_factor, 0.0f, 10.0f);
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		}
		ImGui::End();
		cam.SpawnControlWindow();
		m_Window.Gfx().EndFrame();							//Замена буфера свап чейна
	}
}

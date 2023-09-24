#include "../includes/Application.h"
#include "../render/includes/CVertex.h"
#include "../includes/Window.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_win32.h"
#include "../imgui/imgui_impl_dx11.h"
#include "../imgui/imgui_internal.h"
#include <memory>



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

		m_Window.Gfx().ClearBuffer(0.07f, 0.0f, 0.12f);		//Очистка текущего буфера свап чейна
		m_Window.Gfx().SetCamera(cam.GetMatrix());
		light.Bind(m_Window.Gfx(), cam.GetMatrix());



		nano.Draw(m_Window.Gfx());
		suz.Draw(m_Window.Gfx());


		light.Draw(m_Window.Gfx());
		ImGuiID did = m_Window.Gfx().ShowDocksape();
		ImGuiDockNode* node = ImGui::DockBuilderGetCentralNode(did);
		m_Window.Gfx().CreateViewport(node->Size.x, node->Size.y, node->Pos.x, node->Pos.y);
		m_Window.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, node->Size.y / node->Size.x, 0.5f, 100.0f));

		m_Window.Gfx().ShowMenuBar();
		ImGui::ShowMetricsWindow();
		cam.SpawnControlWindow();
		light.SpawnControlWindow();
		nano.ShowWindow();
		suz.ShowWindow("Model2");
		

		m_Window.Gfx().EndFrame();							//Замена буфера свап чейна
	}


}

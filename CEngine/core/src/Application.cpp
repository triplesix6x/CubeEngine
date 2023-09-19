#include "../includes/Application.h"
#include "../render/includes/Box.h"
#include "../render/includes/Melon.h"
#include "../render/includes/Pyramid.h"
#include "../includes/Window.h"
#include <memory>

namespace Cube
{

	Application::Application(int width, int height) : m_Window(width, height)
	{
		m_Window.WindowShow();
		std::mt19937 rng(std::random_device{}());
		std::uniform_real_distribution<float> adist(0.0f, 3.1415f * 2.0f);
		std::uniform_real_distribution<float> ddist(0.0f, 3.1415f * 2.0f);
		std::uniform_real_distribution<float> odist(0.0f, 3.1415f * 0.3f);
		std::uniform_real_distribution<float> rdist(6.0f, 20.0f);
		std::uniform_real_distribution<float> bdist{ 0.4f,3.0f };
		std::uniform_int_distribution<int> latdist{ 5,20 };
		std::uniform_int_distribution<int> longdist{ 10,40 };
		std::uniform_int_distribution<int> typedist{ 0,2 };
		for (auto i = 0; i < nDrawables; i++)
		{
			drawables.push_back(std::make_unique<Pyramid>(
				m_Window.Gfx(), rng, adist, ddist,
				odist, rdist
			));
			drawables.push_back(std::make_unique<Box>(
				m_Window.Gfx(), rng, adist,
				ddist, odist, rdist, bdist
			));
			drawables.push_back(std::make_unique<Melon>(
				m_Window.Gfx(), rng, adist, ddist,
				odist, rdist, longdist, latdist
			));

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
				return *ecode;
			}
			doFrame();
		}
	}

	void Application::doFrame()
	{
		auto dt = timer.Mark();
		m_Window.Gfx().ClearBuffer(0.07f, 0.0f, 0.12f);
		for (auto& b : drawables)
		{
			b->Update(dt);
			b->Draw(m_Window.Gfx());
		}
		m_Window.Gfx().EndFrame();
	}
}

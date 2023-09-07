#pragma once
#include "Core.h"

namespace Cube
{

	class CUBE_API Application
	{
	public:
		Application();
		virtual ~Application();
		void run();
	};
	Application* CreateApplication();
	void ReleaseApplication(void* app);
}

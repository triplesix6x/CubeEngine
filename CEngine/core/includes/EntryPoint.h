#pragma once
#include <Windows.h>

#ifdef CUBE_PLATFORM_WINDOWS

extern Cube::Application* Cube::CreateApplication();
extern void Cube::ReleaseApplication(void* app);

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Cube::Log::init();
	CUBE_INFO("Hello!");
	auto app = Cube::CreateApplication();
	app->run();
	Cube::ReleaseApplication(app);
	return 0;
}

#endif
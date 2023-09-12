#pragma once

#ifdef CUBE_PLATFORM_WINDOWS

extern Cube::Application* Cube::CreateApplication();
extern void Cube::ReleaseApplication(void* app);

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Cube::Log::init();
	auto app = Cube::CreateApplication();
	int Result = app->run();
	Cube::ReleaseApplication(app);
	return Result;
}

#endif
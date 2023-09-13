#pragma once

#ifdef CUBE_PLATFORM_WINDOWS

extern Cube::Application* Cube::CreateApplication();
extern void Cube::ReleaseApplication(void* app);

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{\
	try 
	{
		Cube::Log::init();
		auto app = Cube::CreateApplication();
		int Result = app->run();
		Cube::ReleaseApplication(app);
		return Result;
	}
	catch (const CubeException& e)
	{
		CUBE_CORE_ERROR("Cube Exception was thrown -> \n{} {}", e.whatEx(), e.getType());
		MessageBoxA(nullptr, e.whatEx(), e.getType(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& e)
	{
		CUBE_CORE_ERROR("Standart Exception was thrown-> \n{} Standart Exception", e.what());
		MessageBoxA(nullptr, e.what(), "Standart Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		CUBE_CORE_ERROR("Unknown Exception was thrown");
		MessageBoxA(nullptr, "No details avalible", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	return -1;
}

#endif
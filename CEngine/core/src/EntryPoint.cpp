#include "../Cube.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	try
	{
		Cube::Log::init();
		Cube::Application app(1600, 900);
		int Result = app.run();
		return Result;
	}
	catch (const CubeException& e)
	{
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


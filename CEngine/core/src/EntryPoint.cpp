//Файл запуска программы

#include "../includes/Log.h"
#include "../includes/Application.h"



int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	HRESULT hResult = CoInitialize(NULL);
	try
	{
		//инициализация логирования
		Cube::Log::init();

		//создание экземпляра приложения
		Cube::Application app(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), Cube::WindowType::CUSTOM);	

		int Result = app.run();																							
		return Result;
	}
	//Обработка исключений
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


//Файл запуска программы

#include "../CubeApp.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	HRESULT hResult = CoInitialize(NULL);
	try
	{
		Cube::Log::init();																		//инициализация логирования
		Cube::Application app(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));	//создание экземпляра приложения
		int Result = app.run();																	//запуск основной функции работы
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


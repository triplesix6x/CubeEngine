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
}


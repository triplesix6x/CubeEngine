//Статичный класс для работы с файлами в Windows

#pragma once
#include <Windows.h>
#include <commdlg.h>
#include <string>

namespace Cube
{
	class FileDialogs
	{
	public:
		//Функция для получения пути существующего файла в виде строки
		static std::string OpenfileA(const char* filter);

		//Функция для получения пути существующего файла в виде широкой строки
		static std::wstring OpenfileW(const wchar_t* filter);

		//Функция для получения пути сохраненного файла в виде строки
		static std::string Savefile(const char* filter);
	};
}
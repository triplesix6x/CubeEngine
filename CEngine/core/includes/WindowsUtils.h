#pragma once
#include <Windows.h>
#include <commdlg.h>
#include <string>

namespace Cube
{
	class FileDialogs
	{
	public:
		static std::string OpenfileA(const char* filter);
		static std::wstring OpenfileW(const wchar_t* filter);
		static std::string Savefile(const char* filter);
	};
}
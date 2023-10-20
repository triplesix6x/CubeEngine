#include "../includes/WindowsUtils.h"
#include "../includes/Log.h"

namespace Cube
{
	std::string FileDialogs::OpenfileA(const char* filter)
	{
		char cname[260] = {0};
		OPENFILENAMEA ofn;
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = NULL;
		ofn.lpstrFile = cname;
		ofn.lpstrFile[0] = '\0';
		ofn.nMaxFile = sizeof(cname);
		ofn.lpstrFilter = filter;
		ofn.lpstrFileTitle = NULL;
		ofn.nFilterIndex = 1;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = "";
		ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
		if (GetOpenFileNameA(&ofn) == TRUE)
		{
			CUBE_INFO(std::string("Opened file  " + std::string(ofn.lpstrFile)).c_str());
			return ofn.lpstrFile;
		}
		CUBE_ERROR(std::string("Failed to open file  " + std::string(ofn.lpstrFile)).c_str());
		return std::string();
	}

	std::wstring FileDialogs::OpenfileW(const wchar_t* filter)
	{
		wchar_t cname[260] = {0};
		OPENFILENAME ofn;
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = NULL;
		ofn.lpstrFile = cname;
		ofn.lpstrFile[0] = '\0';
		ofn.nMaxFile = sizeof(cname);
		ofn.lpstrFilter = filter;
		ofn.lpstrFileTitle = NULL;
		ofn.nFilterIndex = 1;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = L"";
		ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
		if (GetOpenFileName(&ofn) == TRUE)
		{
			return ofn.lpstrFile;
		}
		return std::wstring();
	}

	std::string FileDialogs::Savefile(const char* filter)
	{
		char cname[260] = { 0 };
		OPENFILENAMEA ofn;
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = NULL;
		ofn.lpstrFile = cname;
		ofn.lpstrFile[0] = '\0';
		ofn.nMaxFile = sizeof(cname);
		ofn.lpstrFilter = filter;
		ofn.lpstrFileTitle = NULL;
		ofn.nFilterIndex = 1;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = "";
		ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
		if (GetSaveFileNameA(&ofn) == TRUE)
		{
			CUBE_INFO(std::string("Saved file  " + std::string(ofn.lpstrFile)).c_str());
			return ofn.lpstrFile;
		}
		CUBE_ERROR(std::string("Failed to save file  " + std::string(ofn.lpstrFile)).c_str());
		return std::string();
	}
}
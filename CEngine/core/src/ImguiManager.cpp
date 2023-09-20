#include "../imgui/imgui.h"
#include "../includes/ImguiManager.h"

ImguiManager::ImguiManager()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsClassic();
}

ImguiManager::~ImguiManager()
{
	ImGui::DestroyContext();
}
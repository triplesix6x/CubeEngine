//Класс для сериализации и десериализации сцен

#pragma once
#include "yaml-cpp\yaml.h"
#include "Application.h"


namespace Cube
{
	class SceneSerializer
	{
	public:
		SceneSerializer(Application& app);
		~SceneSerializer() = default;

		//Функция сериализации в файл
		void Serialize(const std::filesystem::path& filepath);

		//Функция десериализации из файла
		bool Deserialize(const std::filesystem::path& filepath);
	private:
		Application* pApp;
	};
}
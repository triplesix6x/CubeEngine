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

		void Serialize(const std::filesystem::path& filepath);
		bool Deserialize(const std::filesystem::path& filepath);
	private:
		Application* pApp;
	};
}
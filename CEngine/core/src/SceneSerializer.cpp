#include "../includes/SceneSerializer.h"
#include "../includes/CXM.h"

#include <fstream>


namespace Cube
{
	YAML::Emitter& operator<<(YAML::Emitter& out, const DirectX::XMFLOAT3& f3)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << f3.x << f3.y << f3.z << YAML::EndSeq;
		return out;
	}


	SceneSerializer::SceneSerializer(Application& app) : pApp(&app)
	{
		
	}

	void SceneSerializer::Serialize(const std::filesystem::path& filepath)
	{

		std::string filename = filepath.filename().string();
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << filename;

		out << YAML::Key << "Models" << YAML::Value << YAML::BeginSeq;
		if (!pApp->models.empty())
		{
			for (int i = 0; i < pApp->models.size(); ++i)
			{
				out << YAML::BeginMap;
				out << YAML::Key << "Model" << YAML::Value << i;

				out << YAML::Key << "Name" << YAML::Value << pApp->models[i]->modelName;
				auto applied = &pApp->models[i]->getpRoot().GetAppliedTransform();
				auto rtranslations = ExtractTranslation(*applied); 
				auto rscales = ExtractScaling(*applied); 
				DirectX::XMFLOAT3 rtc = { rtranslations.x / rscales.x, rtranslations.y / rscales.y, rtranslations.z / rscales.z }; 
				out << YAML::Key << "Root Node Translation" << YAML::Value << rtc; 
				out << YAML::Key << "Root Node Scaling" << YAML::Value << rscales;
				out << YAML::Key << "Child Nodes" << YAML::Value << YAML::BeginSeq;
				for (int j = 0; j < pApp->models[i]->getpRoot().getchildPtrs().size(); ++j)
				{
					out << YAML::BeginMap; 
					auto childapplied = &pApp->models[i]->getpRoot().getchildPtrs()[j]->GetAppliedTransform();
					auto translations = ExtractTranslation(*childapplied);
					auto scales = ExtractScaling(*childapplied);
					DirectX::XMFLOAT3 tc = { translations.x / scales.x, translations.y / scales.y, translations.z / scales.z };
					out << YAML::Key << "Name" << YAML::Value << pApp->models[i]->getpRoot().getchildPtrs()[j]->GetName();
					out << YAML::Key << "Translation" << YAML::Value << tc;
					out << YAML::Key << "Scaling" << YAML::Value << scales;
					out << YAML::EndMap; 
				}

				out << YAML::EndSeq; 
				out << YAML::EndMap;
			}
		}
		out << YAML::EndSeq;

		out << YAML::Key << "Lights" << YAML::Value << YAML::BeginSeq;
		for (int i = 0; i < pApp->light.sceneLights.size(); ++i)
		{
			out << YAML::BeginMap;
			out << YAML::Key << "Light" << YAML::Value << i;

			out << YAML::Key << "Name" << YAML::Value << pApp->light.sceneLights[i]->lightName;

			out << YAML::Key << "Translation" << YAML::Value << pApp->light.sceneLights[i]->getCbuf().pos;
			out << YAML::Key << "Intensity" << YAML::Value << pApp->light.sceneLights[i]->getCbuf().diffuseIntensity;
			out << YAML::Key << "Diffuse Color" << YAML::Value << pApp->light.sceneLights[i]->getCbuf().diffuseColor;
			out << YAML::Key << "Ambient Color" << YAML::Value << pApp->light.sceneLights[i]->getCbuf().ambient;
			out << YAML::Key << "Attenuation Constant" << YAML::Value << pApp->light.sceneLights[i]->getCbuf().attConst;
			out << YAML::Key << "Attenuation Linear" << YAML::Value << pApp->light.sceneLights[i]->getCbuf().attLin;
			out << YAML::Key << "Attenuation Quadratic" << YAML::Value << pApp->light.sceneLights[i]->getCbuf().attQuad;
			out << YAML::Key << "Draw Sphere" << YAML::Value << pApp->light.sceneLights[i]->DrawSphere();

			out << YAML::EndMap;
		}
		out << YAML::EndSeq;
		
		out << YAML::EndMap;
		std::ofstream fout(filepath);
		fout << out.c_str();
	}

	void SceneSerializer::SerializeRuntime(const std::filesystem::path& filepath)
	{

	}

	void SceneSerializer::Deserialize(const std::filesystem::path& filepath)
	{

	}

	void SceneSerializer::DeserializeRuntime(const std::filesystem::path& filepath)
	{

	}
}
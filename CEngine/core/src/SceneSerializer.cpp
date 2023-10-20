#include "../includes/SceneSerializer.h"
#include "../includes/CXM.h"
#include "../includes/CMath.h"
#include "../includes/Log.h"

#include <fstream>



namespace YAML
{
	template<>
	struct convert<DirectX::XMFLOAT3>
	{
		static Node encode(const DirectX::XMFLOAT3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			return node;
		}

		static bool decode(const Node& node, DirectX::XMFLOAT3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;
			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};
}
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

		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << filepath.string();
		out << YAML::Key << "Skybox" << YAML::Value << std::filesystem::relative(pApp->skybox->path, filepath.parent_path()).string();
		out << YAML::Key << "Draw Grid" << YAML::Value << pApp->drawGrid;

		out << YAML::Key << "Models" << YAML::Value << YAML::BeginSeq;
		if (!pApp->models.empty())
		{
			for (int i = 0; i < pApp->models.size(); ++i)
			{
				out << YAML::BeginMap;
				out << YAML::Key << "Model" << YAML::Value << i;

				

				out << YAML::Key << "Name" << YAML::Value << pApp->models[i]->modelName;
				out << YAML::Key << "Path" << YAML::Value << std::filesystem::relative(pApp->models[i]->rootPath, filepath.parent_path()).string(); 
				auto applied = &pApp->models[i]->getpRoot().GetAppliedTransform();
				auto appliedScale = &pApp->models[i]->getpRoot().GetAppliedScale();
				auto rtranslations = ExtractTranslation(*applied); 
				auto rscales = ExtractScaling(*appliedScale);
				auto rangles = ExtractEulerAngles(*applied);
				out << YAML::Key << "Root Node Translation" << YAML::Value << rtranslations;
				out << YAML::Key << "Root Node Scaling" << YAML::Value << rscales;
				out << YAML::Key << "Root Node Angles" << YAML::Value << rangles;
				out << YAML::Key << "Child Nodes" << YAML::Value << YAML::BeginSeq;
				for (int j = 0; j < pApp->models[i]->getpRoot().getchildPtrs().size(); ++j)
				{
					out << YAML::BeginMap; 
					auto childapplied = &pApp->models[i]->getpRoot().getchildPtrs()[j]->GetAppliedTransform();
					auto childappliedScale = &pApp->models[i]->getpRoot().getchildPtrs()[j]->GetAppliedScale();
					auto translations = ExtractTranslation(*childapplied);
					auto scales = ExtractScaling(*childappliedScale);
					auto angles = ExtractEulerAngles(*childapplied);
					out << YAML::Key << "Child" << YAML::Value << j;
					out << YAML::Key << "Translation" << YAML::Value << translations;
					out << YAML::Key << "Scaling" << YAML::Value << scales;
					out << YAML::Key << "Angles" << YAML::Value << angles;
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

		out << YAML::Key << "Cameras" << YAML::Value << YAML::BeginSeq;
		out << YAML::BeginMap;
		
		out << YAML::Key << "Camera" << YAML::Value << "Editor";
		out << YAML::Key << "Translation" << YAML::Value << pApp->cam.pos;
		out << YAML::Key << "Travel Speed" << YAML::Value << pApp->cam.travelSpeed;
		out << YAML::Key << "Pitch" << YAML::Value << pApp->cam.pitch;
		out << YAML::Key << "Yaw" << YAML::Value << pApp->cam.yaw;
		out << YAML::Key << "Rotation Speed" << YAML::Value << pApp->cam.rotationSpeed;

		out << YAML::EndMap;
		out << YAML::EndSeq;
		
		out << YAML::EndMap;
		std::ofstream fout(filepath);
		fout << out.c_str();
		CUBE_INFO("Scene has been successfully saved");
	}

	bool SceneSerializer::Deserialize(const std::filesystem::path& filepath)
	{
		std::ifstream stream(filepath);
		std::stringstream strStream;
		strStream << stream.rdbuf();
		YAML::Node data = YAML::Load(strStream.str());

		if (!data["Scene"])
		{
			CUBE_ERROR("Bad scene file");
			return false;
		}
		std::string sName = data["Scene"].as<std::string>();
		pApp->scenePath = sName;
		pApp->drawGrid = data["Draw Grid"].as<bool>();
		auto models = data["Models"];

		auto skynewabsolute = filepath.parent_path().string() + '\\' + data["Skybox"].as<std::string>(); 
		if (std::filesystem::exists(skynewabsolute))
		{
			pApp->skybox.release();
			pApp->skybox = std::make_unique<SkyBox>(pApp->m_Window.Gfx(), skynewabsolute);
			CUBE_TRACE(std::string("Loaded skybox file  " + skynewabsolute).c_str());
		}
		else
		{
			CUBE_ERROR(std::string("File doesn't exist: " + skynewabsolute).c_str());
		}

		if (models)
		{
			pApp->models.clear();
			for (auto model : models)
			{
				auto newabsolute = filepath.parent_path().string() + '\\' + model["Path"].as<std::string>();
				if (std::filesystem::exists(newabsolute))
				{
					pApp->models.push_back(std::make_unique<Model>(pApp->m_Window.Gfx(), newabsolute,
						model["Model"].as<int>(),
						model["Name"].as<std::string>()));
					pApp->id++;
					DirectX::XMFLOAT3 rtc = model["Root Node Translation"].as<DirectX::XMFLOAT3>();
					DirectX::XMFLOAT3 rsc = model["Root Node Scaling"].as<DirectX::XMFLOAT3>();
					DirectX::XMFLOAT3 ra = model["Root Node Angles"].as<DirectX::XMFLOAT3>();
					pApp->models[model["Model"].as<int>()]->SetRootTransfotm(DirectX::XMMatrixRotationRollPitchYaw(ra.x, ra.y, ra.z) *
						DirectX::XMMatrixTranslation(rtc.x, rtc.y, rtc.z));
					pApp->models[model["Model"].as<int>()]->SetRootScaling(DirectX::XMMatrixScaling(rsc.x, rsc.y, rsc.z));
					auto& childptrs = pApp->models[model["Model"].as<int>()]->getpRoot().getchildPtrs();
					auto childs = model["Child Nodes"];
					for (auto child : childs)
					{
						if (child)
						{
							DirectX::XMFLOAT3 tc = child["Translation"].as<DirectX::XMFLOAT3>();
							DirectX::XMFLOAT3 sc = child["Scaling"].as<DirectX::XMFLOAT3>();
							DirectX::XMFLOAT3 a = child["Angles"].as<DirectX::XMFLOAT3>();
							childptrs[child["Child"].as<int>()]->SetAppliedTransform(DirectX::XMMatrixRotationRollPitchYaw(a.x, a.y, a.z) *
								DirectX::XMMatrixTranslation(tc.x, tc.y, tc.z));
							childptrs[child["Child"].as<int>()]->SetAppliedScale(DirectX::XMMatrixScaling(sc.x, sc.y, sc.z));
						}
						else
						{
							CUBE_ERROR(std::string("Unable to load child " + child["Child"].as<std::string>()));
						}
					}
					CUBE_TRACE(std::string("Loaded model file  " + newabsolute).c_str());
				}
				else
				{
					CUBE_ERROR(std::string("File doesn't exist: " + newabsolute).c_str());
				}
			}
		}

		auto lights = data["Lights"];
		if (lights)
		{
			pApp->light.clearLights();
			for (auto light : lights)
			{
				pApp->light.AddLight(pApp->m_Window.Gfx(), light["Name"].as<std::string>());
				PointLightCBuf Cbuf =
				{
					light["Translation"].as<DirectX::XMFLOAT3>(),
					light["Ambient Color"].as<DirectX::XMFLOAT3>(),
					light["Diffuse Color"].as<DirectX::XMFLOAT3>(),
					light["Intensity"].as<float>(),
					light["Attenuation Constant"].as<float>(),
					light["Attenuation Linear"].as<float>(),
					light["Attenuation Quadratic"].as<float>()
				};
				pApp->light.sceneLights[light["Light"].as<int>()]->setCbuf(Cbuf);
				pApp->light.sceneLights[light["Light"].as<int>()]->drawSphere = light["Draw Sphere"].as<bool>();
			}
		}
		auto cameras = data["Cameras"];
		for (auto camera : cameras)
		{
			pApp->cam.travelSpeed = camera["Travel Speed"].as<float>();
			pApp->cam.pos = camera["Translation"].as<DirectX::XMFLOAT3>(); 
			pApp->cam.yaw = camera["Yaw"].as<float>();
			pApp->cam.pitch = camera["Pitch"].as<float>();
			pApp->cam.rotationSpeed = camera["Rotation Speed"].as<float>();
		}
		CUBE_INFO("Scene has been successfully opened");
	} 
}
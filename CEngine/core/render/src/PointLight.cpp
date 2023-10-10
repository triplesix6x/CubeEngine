#include "../includes/PointLight.h"
#include "../../imgui/imgui.h"

Lights::PointLight::PointLight(Graphics& gfx, int id, float radius, std::string lightName) : id(id), mesh(gfx, radius), lightName(lightName)
{
	Reset();
}

void Lights::PointLight::SpawnControlWindow() noexcept
{
	if (ImGui::TreeNode(&id, lightName.c_str()))
	{
		char buffer[256];
		memset(buffer, 0, sizeof(buffer));
		strcpy_s(buffer, sizeof(buffer), lightName.c_str());
		if (ImGui::InputText("Light Name", buffer, sizeof(buffer)))
		{
			lightName = std::string(buffer);
		}

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 100);
		ImGui::Text("Position");
		ImGui::NextColumn();
		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.3f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });

		if (ImGui::Button("X", buttonSize))
			cbData.pos.x = -3.0f;
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &cbData.pos.x, 0.1f);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });

		if (ImGui::Button("Y", buttonSize))
			cbData.pos.y = 4.0f;
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &cbData.pos.y, 0.1f);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });

		if (ImGui::Button("Z", buttonSize))
			cbData.pos.z = -3.0f;
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &cbData.pos.z, 0.1f);
		ImGui::PopItemWidth();
		ImGui::Columns(1);
		ImGui::PopStyleVar();
		ImGui::Text("Intensity/Color");
		ImGui::SliderFloat("Intensity", &cbData.diffuseIntensity, 0.01f, 20.0f, "%.2f");
		ImGui::ColorEdit3("Diffuse Color", &cbData.diffuseColor.x);
		ImGui::ColorEdit3("Ambient", &cbData.ambient.x);

		ImGui::Text("Falloff");
		ImGui::SliderFloat("Constant", &cbData.attConst, 0.05f, 10.0f, "%.2f");
		ImGui::SliderFloat("Linear", &cbData.attLin, 0.0001f, 4.0f, "%.4f");
		ImGui::SliderFloat("Quadratic", &cbData.attQuad, 0.0000001f, 2.0f, "%.7f");
		ImGui::Checkbox("Draw Light Sphere", &drawSphere);
		if (ImGui::Button("Reset"))
		{
			Reset();
		}
		ImGui::TreePop();
	}
}

bool Lights::PointLight::DrawSphere()
{
	return drawSphere;
}

void Lights::PointLight::Reset() noexcept
{
	cbData = {
		{-3.0f, 4.0f, -3.0f},
		{0.05f, 0.05f, 0.05f},
		{1.0f, 1.0f, 1.0f},
		1.0f,
		1.0f,
		0.045f,
		0.0075f,
	};
}

PointLightCBuf Lights::PointLight::getCbuf() const
{
	return cbData;
}

void Lights::PointLight::Draw(Graphics& gfx) const noexcept
{
	mesh.SetPos(cbData.pos);
	mesh.Draw(gfx);
}

Lights::Lights(Graphics& gfx, float radius) : cbuf(gfx, 0u, 32u)
{
	sceneLights.push_back(std::make_unique<PointLight>(gfx, id, 0.5f, "Point Light (R10)"));
	++id;
	UpdateCbufs();
}

void Lights::AddLight(Graphics& gfx)
{
	if (sceneLights.size() < 32)
	{
		sceneLights.push_back(std::make_unique<PointLight>(gfx, id));
		++id;
		UpdateCbufs();
	}
	else
	{
		MessageBoxA(nullptr, "There can only be 32 light sources in a scene.", "Light error", MB_OK | MB_ICONEXCLAMATION);
	}
}

int Lights::getLighstCount()
{
	return sceneLights.size();
}

int Lights::getId(int i)
{
	return sceneLights[i]->id;
}

void Lights::DeleteLight(int i)
{
	if (sceneLights.size() > 1)
	{
		sceneLights.erase(sceneLights.begin() + i);
		UpdateCbufs();
	}
	else
	{
		MessageBoxA(nullptr, "There must be at least 1 light source.", "Light error", MB_OK | MB_ICONEXCLAMATION);
	}
}

void Lights::clearLights()
{
	if (sceneLights.size() > 1)
	{
		sceneLights.erase(sceneLights.begin() + 1, sceneLights.end());
	}
}


std::string Lights::getName(int i)
{
	return sceneLights[i]->lightName;
}

void Lights::UpdateCbufs()
{
	for (int i = 0; i < sceneLights.size(); ++i)
	{
		cbufs[i] = sceneLights[i]->getCbuf();
	}
}

void Lights::spawnWnds()
{
	for (auto& l : sceneLights)
	{
		l->SpawnControlWindow();
	}
}

void Lights::drawSpheres(Graphics& gfx)
{
	for (auto& l : sceneLights)
	{
		if (l->DrawSphere())
		{
			l->Draw(gfx);
		}
	}
}

void Lights::Bind(Graphics& gfx, DirectX::FXMMATRIX view) noexcept
{
	UpdateCbufs();
    auto dataCopy = cbufs;
	for (int i = 0; i < sizeof(cbufs) / sizeof(cbufs[0]); ++i)
	{
		const auto pos = DirectX::XMLoadFloat3(&cbufs[i].pos);
		DirectX::XMStoreFloat3(&dataCopy[i].pos, DirectX::XMVector3Transform(pos, view));
	}
	cbuf.Update(gfx, dataCopy[0], sizeof(cbufs) / sizeof(cbufs[0]));
	cbuf.Bind(gfx);
}
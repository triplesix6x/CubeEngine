#include "../includes/PointLight.h"
#include "../../imgui/imgui.h"

Lights::PointLight::PointLight(Graphics& gfx, int id, float radius) : id(id), mesh(gfx, radius)
{
	Reset();
}

void Lights::PointLight::SpawnControlWindow() noexcept
{
	std::string name = "Light " + std::to_string(id);
	if (ImGui::TreeNode(name.c_str()))
	{
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

void Lights::PointLight::Bind(Graphics& gfx, DirectX::FXMMATRIX view) const noexcept
{
	auto dataCopy = cbData;
	const auto pos = DirectX::XMLoadFloat3(&cbData.pos);
	DirectX::XMStoreFloat3(&dataCopy.pos, DirectX::XMVector3Transform(pos, view));
}

void Lights::PointLight::Draw(Graphics& gfx) const noexcept
{
	mesh.SetPos(cbData.pos);
	mesh.Draw(gfx);
}

Lights::Lights(Graphics& gfx, float radius) : cbuf(gfx, 0u, 32u)
{
	sceneLights.push_back(std::make_unique<PointLight>(gfx, id));
	++id;
	UpdateCbufs();
}

void Lights::AddLight(Graphics& gfx)
{
	sceneLights.push_back(std::make_unique<PointLight>(gfx, id));
	++id;
	UpdateCbufs();
}

void Lights::DeleteLight(int i)
{
	sceneLights.erase(sceneLights.begin() + i);
	UpdateCbufs();
}

void Lights::UpdateCbufs()
{
	for (int i = 0; i < sceneLights.size(); ++i)
	{
		cbufs[i] = sceneLights[i]->getCbuf();
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
//Файл для создания и загрузки в pipeline сущности куба, для других геометрических фигур принцип такой же

#include "../includes/Box.h"
#include "../includes/TestObject.h"
#include "../includes/BindableBase.h"
#include "../includes/Cube.h"
#include "../imgui/imgui.h"


Box::Box(Graphics& gfx,
	std::mt19937& rng,
	std::uniform_real_distribution<float>& adist,
	std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist,
	std::uniform_real_distribution<float>& rdist,
	std::uniform_real_distribution<float>& bdist,
	DirectX::XMFLOAT3 material)
	:
	TestObject(gfx, rng, adist, ddist, odist, rdist)
{
	namespace dx = DirectX;
	if (!IsStaticInitialized())
	{
		//Матрица с позицией в пронстранстве и нормалью
		struct Vertex
		{
			dx::XMFLOAT3 pos;
			dx::XMFLOAT3 n;
		};
		auto model = Cube::MakeIndependent<Vertex>();
		model.SetNormalsIndependentFlat();
		AddStaticBind(std::make_unique<VertexBuffer>(gfx, model.vertices));

		auto pvs = std::make_unique<VertexShader>(gfx, L"shaders\\PhongVS.cso");
		auto pvsbc = pvs->GetBytecode();
		AddStaticBind(std::move(pvs));

		AddStaticBind(std::make_unique<PixelShader>(gfx, L"shaders\\PhongPS.cso"));
		
		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, model.indices));

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
		};
		AddStaticBind(std::make_unique<InputLayout>(gfx, ied, pvsbc));

		AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	else
	{
		SetIndexFromStatic();
	}
	materialConstants.color = material;
	AddBind(std::make_unique<MaterialCbuf>(gfx, materialConstants, 1u));
	AddBind(std::make_unique<TransformCbuf>(gfx, *this));
	dx::XMStoreFloat3x3(&mt,dx::XMMatrixScaling(1.0f, 1.0f, bdist(rng)));
}


DirectX::XMMATRIX Box::GetTransformXM() const noexcept
{
	namespace dx = DirectX;
	return dx::XMLoadFloat3x3(&mt) * TestObject::GetTransformXM();
}

bool Box::SpawnControlWindow(int id, Graphics& gfx) noexcept
{
	using namespace std::string_literals;

	bool dirty = false;
	bool open = true;
	if (ImGui::Begin(("Box "s + std::to_string(id)).c_str(), &open))
	{
		ImGui::Text("Material");
		const auto cd = ImGui::ColorEdit3("Material Color", &materialConstants.color.x);
		const auto sip = ImGui::SliderFloat("Specular Intensity", &materialConstants.specularIntensity, 0.05f, 4.0f, "%.2f");
		const auto spd = ImGui::SliderFloat("Specular Power", &materialConstants.specularPower, 1.0f, 5.0f, "%.2f");
		dirty = cd || sip || spd;
		ImGui::Text("Position");
		ImGui::SliderFloat("R", &r, 0.0f, 80.0f, "%.1f");
		ImGui::SliderAngle("Theta", &theta, -180.0f, 180.0f);
		ImGui::SliderAngle("Phi", &phi, -180.0f, 180.0f);
		ImGui::Text("Orientation");
		ImGui::SliderAngle("Roll", &roll, -180.0f, 180.0f);
		ImGui::SliderAngle("Pitch", &pitch, -180.0f, 180.0f);
		ImGui::SliderAngle("Yaw", &yaw, -180.0f, 180.0f);
	}
	ImGui::End();

	if (dirty)
	{
		SyncMaterial(gfx);
	}
	return open;
}

void Box::SyncMaterial(Graphics& gfx) noexcept
{
	auto pConstPS = QueryBindable<MaterialCbuf>();
	assert(pConstPS != nullptr);
	pConstPS->Update(gfx, materialConstants);
}
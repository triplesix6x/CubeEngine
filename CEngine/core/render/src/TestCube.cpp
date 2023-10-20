#include "../includes/TestCube.h"
#include "../includes/Cube.h"
#include "../includes/BindableBase.h"
#include "../includes/TransformCbufPS.h"
#include "../imgui/imgui.h"

TestCube::TestCube(Graphics& gfx, float size)
{
	namespace dx = DirectX;

	auto model = CCube::MakeIndependentTextured();
	model.Transform(dx::XMMatrixScaling(size, size, size));
	model.SetNormalsIndependentFlat();
	AddBind(std::make_unique<DepthStencil>(gfx, false));
	AddBind(std::make_unique<Rasterizer>(gfx, false));
	AddBind(std::make_unique<VertexBuffer>(gfx, model.vertices));
	AddIndexBuffer(std::make_unique <IndexBuffer>(gfx, model.indices));

	AddBind(std::make_unique<Texture>(gfx, "models\\brick\\brickwall.jpg"));
	AddBind(std::make_unique<Texture>(gfx, "models\\brick\\brickwall_normal.jpg", 1u));

	auto pvs = std::make_unique<VertexShader>(gfx, L"shaders\\PhongVS.cso");
	auto pvsbc = pvs->GetBytecode();
	AddBind(std::move(pvs));

	AddBind(std::make_unique<PixelShader>(gfx, L"shaders\\PhongNormalMapPS.cso"));

	AddBind(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(gfx, pmc, 1u));

	AddBind(std::make_unique<InputLayout>(gfx, model.vertices.GetLayout().GetD3DLayout(), pvsbc));

	AddBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	AddBind(std::make_unique<TransformCbufPS>(gfx, *this, 0u, 2u));
}

void TestCube::SetPos(DirectX::XMFLOAT3 pos) noexcept
{
	this->pos = pos;
}

void TestCube::SetRotation(float roll, float pitch, float yaw) noexcept
{
	this->roll = roll;
	this->pitch = pitch;
	this->yaw = yaw;
}

DirectX::XMMATRIX TestCube::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw(roll, pitch, yaw) *
		DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
}

void TestCube::spawnControlWindow(Graphics& gfx)
{
	if (ImGui::Begin("Cube"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &pos.x, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Y", &pos.y, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Z", &pos.z, -80.0f, 80.0f, "%.1f");
		ImGui::Text("Orientation");
		ImGui::SliderAngle("Roll", &roll, -180.0f, 180.0f);
		ImGui::SliderAngle("Pitch", &pitch, -180.0f, 180.0f);
		ImGui::SliderAngle("Yaw", &yaw, -180.0f, 180.0f);
		ImGui::Text("Shading");
		bool changed0 = ImGui::SliderFloat("Spec. Int.", &pmc.specularIntensity, 0.0f, 1.0f);
		bool changed1 = ImGui::SliderFloat("Spec. Power", &pmc.specularPower, 0.0f, 100.0f);
		bool checkState = pmc.normalMapEnabled == TRUE;
		bool changed2 = ImGui::Checkbox("Enable Normal Map", &checkState);
		pmc.normalMapEnabled = checkState ? TRUE : FALSE;
		if (changed0 || changed1 || changed2)
		{
			QueryBindable<PixelConstantBuffer<PSMaterialConstant>>()->Update(gfx, pmc);
		}
	}
	ImGui::End();
}
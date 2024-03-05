#include "../includes/TestCube.h"
#include "../includes/Cube.h"
#include "../includes/Bindablebase.h"
#include "../includes/TransformCbufPS.h"
#include "imgui.h"
#include "../includes/DepthStencil.h"
#include "../includes/NullPixelShader.h"

TestCube::TestCube(Graphics& gfx, float size)
{
	namespace dx = DirectX;

	auto model = CCube::MakeIndependentTextured();
	model.Transform(dx::XMMatrixScaling(size, size, size));
	model.SetNormalsIndependentFlat();
	pVertices = std::make_unique<VertexBuffer>(gfx, model.vertices);
	pIndices = std::make_unique<IndexBuffer>(gfx, model.indices);
	pTopology = std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	{
		Technique standard;
		{
			Step Phong(1);

			Phong.AddBindable(std::make_shared<Texture>(gfx, "textures\\brickwall.jpg"));
			Phong.AddBindable(std::make_shared<Sampler>(gfx));

			auto pvs = std::make_shared<VertexShader>(gfx, L"shaders\\PhongVS.cso");
			auto pvsbc = pvs->GetBytecode();
			Phong.AddBindable(std::move(pvs));

			Phong.AddBindable(std::make_shared<PixelShader>(gfx, L"shaders\\PhongPS.cso"));

			Phong.AddBindable(std::make_shared<PixelConstantBuffer<PSMaterialConstant>>(gfx, pmc, 1u));

			Phong.AddBindable(std::make_shared<InputLayout>(gfx, model.vertices.GetLayout(), pvsbc));

			Phong.AddBindable(std::make_unique<TransformCbuf>(gfx));

			standard.AddStep(std::move(Phong));
		}
		AddTechnique(std::move(standard));
	}

	{
		Technique outline;
		{
			Step mask(2);

			auto pvs = std::make_shared<VertexShader>(gfx, L"shaders\\SolidVS.cso");
			auto pvsbc = pvs->GetBytecode();
			mask.AddBindable(std::move(pvs));


			mask.AddBindable(std::make_shared<InputLayout>(gfx, model.vertices.GetLayout(), pvsbc));

			mask.AddBindable(std::make_shared<TransformCbuf>(gfx));



			outline.AddStep(std::move(mask));
		}
		{
			Step draw(3);

			auto pvs = std::make_shared<VertexShader>(gfx, L"shaders\\SolidVS.cso");
			auto pvsbc = pvs->GetBytecode();
			draw.AddBindable(std::move(pvs));

			draw.AddBindable(std::make_shared<PixelShader>(gfx, L"shaders\\SolidPS.cso"));

			draw.AddBindable(std::make_shared<InputLayout>(gfx, model.vertices.GetLayout(), pvsbc));

			class TransformCbufScaling : public TransformCbuf
			{
			public:
				using TransformCbuf::TransformCbuf;
				void Bind(Graphics& gfx) noexcept override
				{
					const auto scale = dx::XMMatrixScaling(1.04f, 1.04f, 1.04f);
					auto xf = GetTransforms(gfx);
					xf.modelview = xf.modelview * scale;
					xf.modelViewProj = xf.modelViewProj * scale;
					UpdateBindImpl(gfx, xf);
				}
			};
			draw.AddBindable(std::make_shared<TransformCbufScaling>(gfx));


			outline.AddStep(std::move(draw));
		}
		AddTechnique(std::move(outline));
	}
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

void TestCube::SpawnControlWindow(Graphics& gfx, const char* name) noexcept
{
	if (ImGui::Begin(name))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &pos.x, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Y", &pos.y, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Z", &pos.z, -80.0f, 80.0f, "%.1f");
		ImGui::Text("Orientation");
		ImGui::SliderAngle("Roll", &roll, -180.0f, 180.0f);
		ImGui::SliderAngle("Pitch", &pitch, -180.0f, 180.0f);
		ImGui::SliderAngle("Yaw", &yaw, -180.0f, 180.0f);
		//	ImGui::Text( "Shading" );
		//	bool changed0 = ImGui::SliderFloat( "Spec. Int.",&pmc.specularIntensity,0.0f,1.0f );
		//	bool changed1 = ImGui::SliderFloat( "Spec. Power",&pmc.specularPower,0.0f,100.0f );
		//	bool checkState = pmc.normalMappingEnabled == TRUE;
		//	bool changed2 = ImGui::Checkbox( "Enable Normal Map",&checkState );
		//	pmc.normalMappingEnabled = checkState ? TRUE : FALSE;
		//	if( changed0 || changed1 || changed2 )
		//	{
		//		QueryBindable<PixelConstantBuffer<PSMaterialConstant>>()->Update( gfx,pmc );
		//	}
	}
	ImGui::End();
}
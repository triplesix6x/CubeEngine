#include "../includes/SkyBox.h"
#include "../includes/BindableBase.h"
#include "../includes/Sphere.h"
#include "../includes/Cube.h"
#include "../includes/SkyBoxTex.h"
#include "../includes/Sampler.h"


SkyBox::SkyBox(Graphics& gfx)
{
	namespace dx = DirectX;
	struct Vertex
	{
		dx::XMFLOAT3 pos;
	};
	if (!IsStaticInitialized())
	{
		AddStaticBind(std::make_unique<Rasterizer>(gfx, true));


		AddStaticBind(std::make_unique<DepthStencil>(gfx));

		auto model = Cube::Make<Vertex>();
		model.Transform(dx::XMMatrixScaling(50.0f, 50.0f, 50.0f));
		AddStaticBind(std::make_unique<VertexBuffer>(gfx, model.vertices));
		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, model.indices));


		AddStaticBind(std::make_unique<SkyBoxTex>(gfx, L"textures\\skybox2.dds"));
		AddStaticBind(std::make_unique<Sampler>(gfx));

		auto pvs = std::make_unique<VertexShader>(gfx, L"shaders\\SkyBoxVS.cso");
		auto pvsbc = pvs->GetBytecode();
		AddStaticBind(std::move(pvs));

		AddStaticBind(std::make_unique<PixelShader>(gfx, L"shaders\\SkyBoxPS.cso"));


		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
		};
		AddStaticBind(std::make_unique<InputLayout>(gfx, ied, pvsbc));

		AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}

	AddBind(std::make_unique<SkyboxTransformCbuf>(gfx));
}

void SkyBox::Update(float dt) noexcept {}


DirectX::XMMATRIX SkyBox::GetTransformXM() const noexcept
{
	return {};
}
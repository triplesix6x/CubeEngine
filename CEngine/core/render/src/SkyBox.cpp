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
	D3D11_DEPTH_STENCIL_DESC dsDesc;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	AddBind(std::make_unique<DepthStencil>(gfx, dsDesc));

	auto model = Cube::Make<Vertex>();
	model.Transform(dx::XMMatrixScaling(5.0f, 5.0f, 5.0f));
	AddBind(std::make_unique<VertexBuffer>(gfx, model.vertices));
	AddIndexBuffer(std::make_unique<IndexBuffer>(gfx, model.indices));


	AddBind(std::make_unique<SkyBoxTex>(gfx, L"textures\\earth.dds"));
	AddBind(std::make_unique<Sampler>(gfx));

	auto pvs = std::make_unique<VertexShader>(gfx, L"shaders\\SkyBoxVS.cso");
	auto pvsbc = pvs->GetBytecode();
	AddBind(std::move(pvs));

	AddBind(std::make_unique<PixelShader>(gfx, L"shaders\\SkyBoxPS.cso"));


	const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
	};
	AddBind(std::make_unique<InputLayout>(gfx, ied, pvsbc));

	AddBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	AddBind(std::make_unique<SkyboxTransformCbuf>(gfx, *this));
}

void SkyBox::Update(float dt) noexcept {}


DirectX::XMMATRIX SkyBox::GetTransformXM() const noexcept
{
	return {};
}
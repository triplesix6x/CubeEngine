#include "../includes/SkyBox.h"
#include "../includes/BindableBase.h"
#include "../includes/Sphere.h"
#include "../includes/Cube.h"
#include "../includes/SkyBoxTex.h"
#include "../includes/Sampler.h"


SkyBox::SkyBox(Graphics& gfx, const wchar_t* name)
{
	namespace dx = DirectX;
	struct Vertex
	{
		dx::XMFLOAT3 pos;
	};


		AddBind(std::make_unique<Rasterizer>(gfx, true));
		AddBind(std::make_unique<DepthStencil>(gfx, true));
		auto model = Cube::Make<Vertex>();
		AddBind(std::make_unique<VertexBuffer>(gfx, model.vertices));
		AddIndexBuffer(std::make_unique<IndexBuffer>(gfx, model.indices));


		AddBind(std::make_unique<SkyBoxTex>(gfx, name));
		AddBind(std::make_unique<Sampler>(gfx));
		
		wchar_t path[260];
		GetModuleFileName(NULL, path, 260);
		int len = wcslen(path);
		for (int i = 1; i < 15; ++i)
		{
			path[len - i] = '\0';
		}

		auto pvs = std::make_unique<VertexShader>(gfx, wcscat(path, L"shaders\\SkyBoxVS.cso"));
		auto pvsbc = pvs->GetBytecode();
		AddBind(std::move(pvs));

		wchar_t bpath[260];
		GetModuleFileName(NULL, bpath, 260);
		int blen = wcslen(bpath);
		for (int i = 1; i < 15; ++i)
		{
			bpath[blen - i] = '\0';
		}

		AddBind(std::make_unique<PixelShader>(gfx, wcscat(bpath, L"shaders\\SkyBoxPS.cso")));

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
		};
		AddBind(std::make_unique<InputLayout>(gfx, ied, pvsbc));

		AddBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	AddBind(std::make_unique<SkyboxTransformCbuf>(gfx));
}

void SkyBox::Update(float dt) noexcept {}


DirectX::XMMATRIX SkyBox::GetTransformXM() const noexcept
{
	return {};
}
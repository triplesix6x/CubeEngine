#include "../includes/SkyBox.h"
#include "../includes/BindableBase.h"
#include "../includes/Sphere.h"
#include "../includes/Cube.h"
#include "../includes/CVertex.h"
#include "../includes/IndexedTriangleList.h"



SkyBox::SkyBox(Graphics& gfx, std::string name) : path(name)
{
	namespace dx = DirectX;

	auto model = CCube::Make();
	pVertices = std::make_shared<VertexBuffer>(gfx, model.vertices);
	pIndices = std::make_shared<IndexBuffer>(gfx, model.indices);
	pTopology = std::make_shared<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	{
		Technique skybox;
		Step only(0);

		auto pvs = std::make_shared<VertexShader>(gfx, L"shaders\\SkyBoxVS.cso");
		auto pvsbc = pvs->GetBytecode();
		only.AddBindable(std::move(pvs));

		only.AddBindable(std::make_shared<PixelShader>(gfx, L"shaders\\SkyBoxPS.cso"));


		only.AddBindable(std::make_shared<InputLayout>(gfx, model.vertices.GetLayout(), pvsbc));
		
		only.AddBindable(std::make_shared<Texture>(gfx, name));
		only.AddBindable(std::make_shared<Sampler>(gfx));
		only.AddBindable(std::make_shared<SkyboxTransformCbuf>(gfx));

		only.AddBindable(std::make_shared<Rasterizer>(gfx, true));

		skybox.AddStep(std::move(only));
		AddTechnique(std::move(skybox));
	}
}


DirectX::XMMATRIX SkyBox::GetTransformXM() const noexcept
{
	return {};
}
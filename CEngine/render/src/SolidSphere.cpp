#include "../includes/SolidSphere.h"
#include "../includes/BindableBase.h"
#include "../includes/Sphere.h"
#include "../includes/CVertex.h"
#include "../includes/IndexedTriangleList.h"


SolidSphere::SolidSphere(Graphics& gfx, float radius)
{
	namespace dx = DirectX;
	auto model = Sphere::Make();
	model.Transform(dx::XMMatrixScaling(radius, radius, radius));
	pVertices = std::make_shared<VertexBuffer>(gfx, model.vertices);
	pIndices = std::make_shared<IndexBuffer>(gfx, model.indices);
	pTopology = std::make_shared<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	{
		Technique solid;
		Step only(1);

		auto pvs = std::make_shared<VertexShader>(gfx, L"shaders\\SolidVS.cso");
		auto pvsbc = pvs->GetBytecode();
		only.AddBindable(std::move(pvs));

		only.AddBindable(std::make_shared<PixelShader>(gfx, L"shaders\\SolidPS.cso"));

		struct PSColorConstant
		{
			dx::XMFLOAT3 color = { 1.0f,1.0f,1.0f };
			float padding;
		} colorConst;
		only.AddBindable(std::make_shared<PixelConstantBuffer<PSColorConstant>>(gfx, colorConst, 1u));

		only.AddBindable(std::make_shared<InputLayout>(gfx, model.vertices.GetLayout(), pvsbc));

		only.AddBindable(std::make_shared<TransformCbuf>(gfx));

		only.AddBindable(std::make_shared<Blender>(gfx, false));

		only.AddBindable(std::make_shared<Rasterizer>(gfx, false));

		solid.AddStep(std::move(only));
		AddTechnique(std::move(solid));
	}
}


void SolidSphere::SetPos(DirectX::XMFLOAT3 pos) noexcept
{
	this->pos = pos;
}

DirectX::XMMATRIX SolidSphere::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
}
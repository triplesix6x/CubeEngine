#include "../includes/SkyboxTransformCbuf.h"

SkyboxTransformCbuf::SkyboxTransformCbuf(Graphics& gfx, const Drawable& parent, UINT slot)
	:
	parent(parent), pVcbuf{std::make_unique<VertexConstantBuffer<Transforms>>(gfx, slot)}
{
}

void SkyboxTransformCbuf::Bind(Graphics& gfx)  noexcept
{
	const Transforms tf =
	{
		DirectX::XMMatrixTranspose(gfx.GetCamera() * gfx.GetProjection())
	};
	pVcbuf->Update(gfx, tf);
	pVcbuf->Bind(gfx);
}

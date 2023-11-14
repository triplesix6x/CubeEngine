#include "../includes/Rasterizer.h"

Rasterizer::Rasterizer(Graphics& gfx, bool twoSided)
{
	D3D11_RASTERIZER_DESC desc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
	desc.CullMode = twoSided ? D3D11_CULL_NONE : D3D11_CULL_BACK;
	GetDevice(gfx)->CreateRasterizerState(&desc, &RSCull);
}

void Rasterizer::Bind(Graphics& gfx)  noexcept
{
	GetContext(gfx)->RSSetState(RSCull.Get());
}
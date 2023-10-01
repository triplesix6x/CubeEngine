#include "../includes/Rasterizer.h"

Rasterizer::Rasterizer(Graphics& gfx, D3D11_RASTERIZER_DESC desc)
{

	GetDevice(gfx)->CreateRasterizerState(&desc, &RSCullNone);
}

void Rasterizer::Bind(Graphics& gfx)  noexcept
{
	GetContext(gfx)->RSSetState(RSCullNone.Get());
}
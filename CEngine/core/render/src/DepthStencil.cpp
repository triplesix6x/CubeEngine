#include "../includes/DepthStencil.h"

DepthStencil::DepthStencil(Graphics& gfx, D3D11_DEPTH_STENCIL_DESC desc)
{

	GetDevice(gfx)->CreateDepthStencilState(&desc, &pDss);
}

void DepthStencil::Bind(Graphics& gfx)  noexcept
{
	GetContext(gfx)->OMSetDepthStencilState(pDss.Get(), 0);
}
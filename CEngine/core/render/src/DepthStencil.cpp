#include "../includes/DepthStencil.h"

DepthStencil::DepthStencil(Graphics& gfx)
{
	D3D11_DEPTH_STENCIL_DESC dsDesc /*= CD3D11_DEPTH_STENCIL_DESC{ CD3D11_DEPTH_STENCIL_DESC{} }*/;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	GetDevice(gfx)->CreateDepthStencilState(&dsDesc, &pDss);
}

void DepthStencil::Bind(Graphics& gfx)  noexcept
{
	GetContext(gfx)->OMSetDepthStencilState(pDss.Get(), 0xFF);
}
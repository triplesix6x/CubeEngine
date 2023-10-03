#include "../includes/DepthStencil.h"

DepthStencil::DepthStencil(Graphics& gfx, bool skybox)
{
	D3D11_DEPTH_STENCIL_DESC dsDesc = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT{});
	if (skybox)
	{
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	}
	else
	{
		dsDesc.DepthEnable = TRUE;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsDesc.StencilEnable = TRUE;
		dsDesc.StencilWriteMask = 0xFF;
		dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	}
	GetDevice(gfx)->CreateDepthStencilState(&dsDesc, &pDss);
}

void DepthStencil::Bind(Graphics& gfx)  noexcept
{
	GetContext(gfx)->OMSetDepthStencilState(pDss.Get(), 0xFF);
}
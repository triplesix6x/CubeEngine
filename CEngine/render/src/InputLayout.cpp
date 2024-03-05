#include "../includes/InputLayout.h"

InputLayout::InputLayout(Graphics& gfx, CubeR::VertexLayout layout_in,ID3DBlob* pVertexShaderBytecode) : layout(std::move(layout_in))
{
		const auto d3dLayout = layout.GetD3DLayout();

		GetDevice(gfx)->CreateInputLayout(
		d3dLayout.data(), (UINT)d3dLayout.size(),
		pVertexShaderBytecode->GetBufferPointer(),
		pVertexShaderBytecode->GetBufferSize(),
		&pInputLayout
	);
}

const CubeR::VertexLayout InputLayout::GetLayout() const noexcept
{
	return layout;
}

void InputLayout::Bind(Graphics& gfx)  noexcept
{
	GetContext(gfx)->IASetInputLayout(pInputLayout.Get());
}
#include "../includes/VertexBuffer.h"



const CubeR::VertexLayout& VertexBuffer::GetLayout() const noexcept
{
	return layout;
}

void VertexBuffer::Bind(Graphics& gfx) noexcept
{
	const UINT offset = 0u;
	GetContext(gfx)->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);
}
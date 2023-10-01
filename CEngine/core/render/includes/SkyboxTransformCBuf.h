#pragma once
#include "ConstantBuffers.h"
#include "Drawable.h"
#include <DirectXMath.h>

class SkyboxTransformCbuf : public Bindable
{
private:
	struct Transforms
	{
		DirectX::XMMATRIX viewProj;
	};
public:
	SkyboxTransformCbuf(Graphics& gfx, const Drawable& parent, UINT slot = 0u);
	void Bind(Graphics& gfx)  noexcept override;
private:
	std::unique_ptr<VertexConstantBuffer<Transforms>> pVcbuf;
	const Drawable& parent;
};
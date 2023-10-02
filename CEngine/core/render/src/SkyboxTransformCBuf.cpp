#include "../includes/SkyboxTransformCbuf.h"


	SkyboxTransformCbuf::SkyboxTransformCbuf(Graphics& gfx, UINT slot)
		:
		pVcbuf{ std::make_unique<VertexConstantBuffer<Transforms>>(gfx,slot) }
	{}

	void SkyboxTransformCbuf::Bind(Graphics& gfx) noexcept
	{
		UpdateBindImpl(gfx, GetTransforms(gfx));
	}

	void SkyboxTransformCbuf::UpdateBindImpl(Graphics& gfx, const Transforms& tf) 
	{
		pVcbuf->Update(gfx, tf);
		pVcbuf->Bind(gfx);
	}

	SkyboxTransformCbuf::Transforms SkyboxTransformCbuf::GetTransforms(Graphics& gfx) 
	{
		return {
			DirectX::XMMatrixTranspose(gfx.GetCamera() * gfx.GetProjection())
		};
	}

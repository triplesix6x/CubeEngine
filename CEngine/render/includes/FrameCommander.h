#pragma once

#include <array>
#include "BindableBase.h"
#include "Graphics.h"
#include "Job.h"
#include "Pass.h"




class FrameCommander

{

public:

	void Accept(Job job, size_t target) noexcept
	{

		passes[target].Accept(job);
	}

	void Execute(Graphics& gfx) const noexcept
	{

		std::make_shared<DepthStencil>(gfx, DepthStencil::Mode::skybox)->Bind(gfx);
		passes[0].Execute(gfx);

		std::make_shared<DepthStencil>(gfx, DepthStencil::Mode::Off)->Bind(gfx);
		passes[1].Execute(gfx);

		std::make_shared<DepthStencil>(gfx, DepthStencil::Mode::Write)->Bind(gfx);
		std::make_shared<NullPixelShader>(gfx)->Bind(gfx);
		passes[2].Execute(gfx);


		std::make_shared<DepthStencil>(gfx, DepthStencil::Mode::Mask)->Bind(gfx);

		struct SolidColorBuffer
		{
			DirectX::XMFLOAT4 color = { 1.0f,0.4f,0.4f,1.0f };
		} scb;
		std::make_shared<PixelConstantBuffer<SolidColorBuffer>>(gfx, scb, 1u)->Bind(gfx);

		passes[3].Execute(gfx);
	}

	void Reset() noexcept
	{
		for (auto& p : passes)
		{
			p.Reset();
		}
	}

private:
	std::array<Pass, 4> passes;
};
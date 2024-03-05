#pragma once
#include "Bindable.h"


	class NullPixelShader : public Bindable
	{
	public:
		NullPixelShader(Graphics& gfx);
		void Bind(Graphics& gfx) noexcept override;
		static std::shared_ptr<NullPixelShader> Resolve(Graphics& gfx);
	};

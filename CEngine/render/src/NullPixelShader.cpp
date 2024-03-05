#include "../includes/NullPixelShader.h"



	NullPixelShader::NullPixelShader(Graphics& gfx)
	{
	}
	void NullPixelShader::Bind(Graphics& gfx) noexcept
	{
		GetContext(gfx)->PSSetShader(nullptr, nullptr, 0u);
	}
#pragma once
#include "DrawableBase.h"

class SkyBox : public DrawableBase<SkyBox>
{
public:
	SkyBox(Graphics& gfx, const wchar_t* name);
	void Update(float dt) noexcept override;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
};
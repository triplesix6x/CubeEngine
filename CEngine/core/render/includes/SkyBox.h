#pragma once
#include "DrawableBase.h"

class SkyBox : public DrawableBase<SkyBox>
{
public:
	SkyBox(Graphics& gfx);
	void Update(float dt) noexcept override;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	DirectX::XMFLOAT3 pos = { 0.0f,0.0f,-10.0f };
};
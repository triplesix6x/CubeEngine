#pragma once
#include "DrawableBase.h"

class SkyBox : public DrawableBase<SkyBox>
{
public:
	SkyBox(Graphics& gfx, std::string name);
	void Update(float dt) noexcept override;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	std::string path;
};
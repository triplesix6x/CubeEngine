// ласс объекта скайбокса. явл€етс€ дочерним классом Drawable

#pragma once
#include "Drawable.h"

class SkyBox : public Drawable
{
public:
	SkyBox(Graphics& gfx, std::string name);
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	std::string path;
};
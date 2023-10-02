#pragma once
#include "Bindable.h"

class DepthStencil : public Bindable
{
public:
	DepthStencil(Graphics& gfx);
	void Bind(Graphics& gfx)  noexcept override;
protected:
	wrl::ComPtr <ID3D11DepthStencilState> pDss;
};
//Смешение DirectX (Bindable)

#pragma once
#include "Bindable.h"
#include <array>


class Blender : public Bindable
{
public:
	Blender(Graphics& gfx, bool blending);
	void Bind(Graphics& gfx) noexcept override;
protected:
	Microsoft::WRL::ComPtr<ID3D11BlendState> pBlender;
	bool blending;
};

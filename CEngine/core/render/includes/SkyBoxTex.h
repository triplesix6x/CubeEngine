#pragma once
#include "Bindable.h"

class SkyBoxTex : public Bindable
{
public:
	SkyBoxTex(Graphics& gfx, const wchar_t* name);
	void Bind(Graphics& gfx) noexcept;
protected:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
};
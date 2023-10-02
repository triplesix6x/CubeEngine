#pragma once
#include "Bindable.h"

class SkyBoxTex : public Bindable
{
public:
	SkyBoxTex(Graphics& gfx, const wchar_t* name);
	void Bind(Graphics& gfx) noexcept;
protected:
	ID3D11Texture2D* cubeTexture = NULL;
	wrl::ComPtr<ID3D11ShaderResourceView> shaderResourceView = NULL;
	D3D11_SHADER_RESOURCE_VIEW_DESC SMViewDesc = {};
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
};
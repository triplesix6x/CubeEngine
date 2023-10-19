#pragma once
#include "Bindable.h"

class DDSTex: public Bindable
{
public:
	DDSTex(Graphics& gfx, std::string path);
	void Bind(Graphics& gfx) noexcept;
protected:
	ID3D11Texture2D* cubeTexture = NULL;
	wrl::ComPtr<ID3D11ShaderResourceView> shaderResourceView = NULL;
	D3D11_SHADER_RESOURCE_VIEW_DESC SMViewDesc = {};
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
};
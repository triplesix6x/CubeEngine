#pragma once
#include "Bindable.h"

class Texture : public Bindable
{
public:
	Texture(Graphics& gfx, const std::string name, unsigned int slot = 0);
	void Bind(Graphics& gfx) noexcept;
private:
	unsigned int slot;
protected:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
};
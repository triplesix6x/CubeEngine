#pragma once
#include "Bindable.h"

class Surface;

class Texture : public Bindable
{
public:
	Texture(Graphics& gfx, const std::string name, unsigned int slot = 0);
	void Bind(Graphics& gfx) noexcept;
	bool HasAlpha() const noexcept;
private:
	unsigned int slot;
protected:
	bool hasAlpha = false;
private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
};
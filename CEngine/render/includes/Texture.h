#pragma once
#include "Bindable.h"
#include "../core/includes/Log.h"


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
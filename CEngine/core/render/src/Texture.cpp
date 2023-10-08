#include "../includes/Texture.h"
#include <WICTextureLoader.h>
#define GFX_THROW_FAILED(hResultcall) if (FAILED(hResult=(hResultcall))) throw Graphics::GraphicsException(__LINE__, __FILE__, hResult)


namespace wrl = Microsoft::WRL;

Texture::Texture(Graphics& gfx, const std::string name, unsigned int slot) : slot(slot)
{
	std::wstring wname = std::wstring(name.begin(), name.end());

	DirectX::CreateWICTextureFromFile(gfx.pDevice.Get(), gfx.pContext.Get(), wname.c_str(), nullptr, pTextureView.GetAddressOf());
}

void Texture::Bind(Graphics& gfx) noexcept
{
	GetContext(gfx)->PSSetShaderResources(slot, 1, pTextureView.GetAddressOf());
}
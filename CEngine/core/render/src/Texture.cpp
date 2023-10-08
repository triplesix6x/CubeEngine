#include "../includes/Texture.h"
#include <WICTextureLoader.h>
#define GFX_THROW_FAILED(hResultcall) if (FAILED(hResult=(hResultcall))) throw Graphics::GraphicsException(__LINE__, __FILE__, hResult)


namespace wrl = Microsoft::WRL;

Texture::Texture(Graphics& gfx, const std::string name, unsigned int slot) : slot(slot)
{
	std::wstring wname = std::wstring(name.begin(), name.end());

	DirectX::CreateWICTextureFromFileEx(gfx.pDevice.Get(), gfx.pContext.Get(), wname.c_str(), 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_DIMENSION_TEXTURE2D, DirectX::DX11::WIC_LOADER_FORCE_RGBA32, nullptr, pTextureView.GetAddressOf());
}


void Texture::Bind(Graphics& gfx) noexcept
{
	GetContext(gfx)->PSSetShaderResources(slot, 1, pTextureView.GetAddressOf());
}
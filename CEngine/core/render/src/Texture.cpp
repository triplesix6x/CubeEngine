#include "../includes/Texture.h"
#include <WICTextureLoader.h>


namespace wrl = Microsoft::WRL;

Texture::Texture(Graphics& gfx, const wchar_t* name)
{
	HRESULT hresult = DirectX::CreateWICTextureFromFile(gfx.pDevice.Get(), gfx.pContext.Get(), name, nullptr, pTextureView.GetAddressOf());
}

void Texture::Bind(Graphics& gfx) noexcept
{
	GetContext(gfx)->PSSetShaderResources(0, 1, pTextureView.GetAddressOf());
}
#include "../includes/SkyBoxTex.h"
#include <DDSTextureLoader.h>
#include <WICTextureLoader.h>
#define GFX_THROW_FAILED(hResultcall) if (FAILED(hResult=(hResultcall))) throw Graphics::GraphicsException(__LINE__, __FILE__, hResult)


namespace wrl = Microsoft::WRL;

SkyBoxTex::SkyBoxTex(Graphics& gfx, const wchar_t* name)
{
	HRESULT hResult;
	/*GFX_THROW_FAILED(*/DirectX::CreateDDSTextureFromFileEx(gfx.pDevice.Get(), gfx.pContext.Get(), name,0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_TEXTURECUBE, DirectX::DX11::DDS_LOADER_DEFAULT, nullptr, pTextureView.GetAddressOf())/*)*/;
}

void SkyBoxTex::Bind(Graphics& gfx) noexcept
{
	GetContext(gfx)->PSSetShaderResources(0, 1, pTextureView.GetAddressOf());
}
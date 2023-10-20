#include "../includes/DDSTex.h"
#include <DDSTextureLoader.h>
#include <WICTextureLoader.h>


namespace wrl = Microsoft::WRL;

DDSTex::DDSTex(Graphics& gfx, std::string path)
{
	HRESULT hResult;
	std::wstring name = std::wstring(path.begin(), path.end());
	if (FAILED(DirectX::CreateDDSTextureFromFileEx(gfx.pDevice.Get(), gfx.pContext.Get(), name.c_str(), 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_TEXTURECUBE, DirectX::DX11::DDS_LOADER_DEFAULT, nullptr, pTextureView.GetAddressOf())))
	{
		CUBE_ERROR(std::string("Failed to load a texture ") + path);
	}
	else
	{
		CUBE_TRACE(std::string("Successfully loaded texture ") + path);
	}
}

void DDSTex::Bind(Graphics& gfx) noexcept
{
	GetContext(gfx)->PSSetShaderResources(0, 1, pTextureView.GetAddressOf());
}
#include "../includes/SkyBoxTex.h"
#include <DDSTextureLoader.h>
#include <WICTextureLoader.h>
#define GFX_THROW_FAILED(hResultcall) if (FAILED(hResult=(hResultcall))) throw Graphics::GraphicsException(__LINE__, __FILE__, hResult)


namespace wrl = Microsoft::WRL;

SkyBoxTex::SkyBoxTex(Graphics& gfx, const wchar_t* name)
{
    //std::vector<std::string> paths = { "textures\\posx.jpg",  "textures\\negx.jpg",
    // "textures\\posy.jpg",
    // "textures\\negy.jpg",
    //"textures\\posz.jpg", "textures\\negz.jpg" };

    //D3D11_TEXTURE2D_DESC texDesc = {};

    //D3D11_TEXTURE2D_DESC texDesc1 = {};


    //ID3D11Texture2D* tex[6] = { nullptr, nullptr, nullptr,nullptr, nullptr, nullptr };
    //for (int i = 0; i < 6; i++)
    //{
    //    std::wstring pathWString(paths[i].begin(), paths[i].end());

    //    HRESULT hr = DirectX::CreateWICTextureFromFileEx(gfx.pDevice.Get(), pathWString.c_str(), 0, D3D11_USAGE_STAGING, 0, D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE, 0,
    //        DirectX::DX11::WIC_LOADER_FLAGS::WIC_LOADER_DEFAULT,
    //        (ID3D11Resource**)&tex[i], 0);
    //    assert(SUCCEEDED(hr));
    //}

    //tex[0]->GetDesc(&texDesc1);

    //texDesc.Width = texDesc1.Width;
    //texDesc.Height = texDesc1.Height;
    //texDesc.MipLevels = texDesc1.MipLevels;
    //texDesc.ArraySize = 6;
    //texDesc.Format = texDesc1.Format;
    //texDesc.CPUAccessFlags = 0;
    //texDesc.SampleDesc.Count = 1;
    //texDesc.SampleDesc.Quality = 0;
    //texDesc.Usage = D3D11_USAGE_DEFAULT;
    //texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    //texDesc.CPUAccessFlags = 0;
    //texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

    //SMViewDesc.Format = texDesc.Format;
    //SMViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
    //SMViewDesc.TextureCube.MipLevels = texDesc.MipLevels;
    //SMViewDesc.TextureCube.MostDetailedMip = 0;


    //GetDevice(gfx)->CreateTexture2D(&texDesc, NULL, &cubeTexture);
    //for (int i = 0; i < 6; i++)
    //{

    //    for (UINT mipLevel = 0; mipLevel < texDesc.MipLevels; ++mipLevel)
    //    {
    //        D3D11_MAPPED_SUBRESOURCE mappedTex2D;
    //        HRESULT hr = (GetContext(gfx)->Map(tex[i], mipLevel, D3D11_MAP_READ, 0, &mappedTex2D));
    //        assert(SUCCEEDED(hr));
    //        GetContext(gfx)->UpdateSubresource(cubeTexture,
    //            D3D11CalcSubresource(mipLevel, i, texDesc.MipLevels),
    //            0, mappedTex2D.pData, mappedTex2D.RowPitch, mappedTex2D.DepthPitch);

    //        GetContext(gfx)->Unmap(tex[i], mipLevel);
    //    }



    //}
    //GetDevice(gfx)->CreateShaderResourceView(cubeTexture, &SMViewDesc, shaderResourceView.GetAddressOf());

	HRESULT hResult;
	GFX_THROW_FAILED(DirectX::CreateDDSTextureFromFileEx(gfx.pDevice.Get(), gfx.pContext.Get(), name,0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_TEXTURECUBE, DirectX::DX11::DDS_LOADER_DEFAULT, nullptr, pTextureView.GetAddressOf()));
}

void SkyBoxTex::Bind(Graphics& gfx) noexcept
{
	GetContext(gfx)->PSSetShaderResources(0, 1, pTextureView.GetAddressOf());
	//GetContext(gfx)->PSSetShaderResources(0, 1, shaderResourceView.GetAddressOf());
}
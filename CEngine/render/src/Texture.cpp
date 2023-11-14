#include "../includes/Texture.h"
#include "DirectXTex.h"
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>
#include <filesystem>


namespace wrl = Microsoft::WRL;

Texture::Texture(Graphics & gfx, const std::string name, unsigned int slot) : slot(slot)
{
	std::wstring wname = std::wstring(name.begin(), name.end());

	std::filesystem::path x(name);

	if (x.extension().string() == ".tga")
	{
		DirectX::TexMetadata metadata;
		DirectX::ScratchImage image;
		DirectX::ScratchImage fimage;
		if (SUCCEEDED(DirectX::LoadFromTGAFile(wname.c_str(), &metadata, image)))
		{
			hasAlpha = !image.IsAlphaAllOpaque();

				if (image.GetImage(0, 0, 0)->format != DXGI_FORMAT_B8G8R8A8_UNORM)
				{
					DirectX::ScratchImage converted;
						DirectX::Convert(*image.GetImage(0, 0, 0), DXGI_FORMAT_B8G8R8A8_UNORM, DirectX::TEX_FILTER_DEFAULT, DirectX::TEX_THRESHOLD_DEFAULT, converted);
						fimage = std::move(converted);
				}
				else
				{
					fimage = std::move(image);
				}

			D3D11_TEXTURE2D_DESC textureDesc = {};
			textureDesc.Width = fimage.GetMetadata().width;
			textureDesc.Height = fimage.GetMetadata().height;
			textureDesc.MipLevels = 0;
			textureDesc.ArraySize = 1;
			textureDesc.Format = fimage.GetImages()->format;
			textureDesc.SampleDesc.Count = 1;
			textureDesc.SampleDesc.Quality = 0;
			textureDesc.Usage = D3D11_USAGE_DEFAULT;
			textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
			textureDesc.CPUAccessFlags = 0;
			textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
			wrl::ComPtr<ID3D11Texture2D> pTexture;
			GetDevice(gfx)->CreateTexture2D(&textureDesc, nullptr, &pTexture);

			GetContext(gfx)->UpdateSubresource(pTexture.Get(), 0u, nullptr, fimage.GetPixels(), fimage.GetImages()->rowPitch, 0u);

			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Format = textureDesc.Format;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MostDetailedMip = 0;
			srvDesc.Texture2D.MipLevels = -1;
			GetDevice(gfx)->CreateShaderResourceView(pTexture.Get(), &srvDesc, &pTextureView);
			GetContext(gfx)->GenerateMips(pTextureView.Get());
			CUBE_TRACE(std::string("Successfully loaded texture ") + name);
		}
		else
		{
			CUBE_ERROR(std::string("Failed to load a texture ") + name);
		}
	}
	else if (x.extension().string() == ".dds")
	{

		if (SUCCEEDED(DirectX::CreateDDSTextureFromFileEx(gfx.pDevice.Get(), gfx.pContext.Get(), wname.c_str(), 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0, DirectX::DX11::DDS_LOADER_DEFAULT, nullptr, pTextureView.GetAddressOf())))
		{
			CUBE_TRACE(std::string("Successfully loaded texture ") + name);
		}
		else
		{
			CUBE_ERROR(std::string("Failed to load a texture ") + name);
		}
	}
	else
	{
		DirectX::TexMetadata metadata;
		DirectX::ScratchImage image;
		DirectX::ScratchImage fimage;
		if (SUCCEEDED(DirectX::LoadFromWICFile(wname.c_str(), DirectX::WIC_FLAGS_NONE, &metadata, image)))
		{
			hasAlpha = !image.IsAlphaAllOpaque();

			if (image.GetImage(0, 0, 0)->format != DXGI_FORMAT_B8G8R8A8_UNORM)
			{
				DirectX::ScratchImage converted;
				DirectX::Convert(*image.GetImage(0, 0, 0), DXGI_FORMAT_B8G8R8A8_UNORM, DirectX::TEX_FILTER_DEFAULT, DirectX::TEX_THRESHOLD_DEFAULT, converted);
				fimage = std::move(converted);
			}
			else
			{
				fimage = std::move(image);
			}
			D3D11_TEXTURE2D_DESC textureDesc = {};
			textureDesc.Width = fimage.GetMetadata().width;
			textureDesc.Height = fimage.GetMetadata().height;
			textureDesc.MipLevels = 0;
			textureDesc.ArraySize = 1;
			textureDesc.Format = fimage.GetImages()->format;
			textureDesc.SampleDesc.Count = 1;
			textureDesc.SampleDesc.Quality = 0;
			textureDesc.Usage = D3D11_USAGE_DEFAULT;
			textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
			textureDesc.CPUAccessFlags = 0;
			textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
			wrl::ComPtr<ID3D11Texture2D> pTexture;
			GetDevice(gfx)->CreateTexture2D(&textureDesc, nullptr, &pTexture);

			GetContext(gfx)->UpdateSubresource(pTexture.Get(), 0u, nullptr, fimage.GetPixels(), fimage.GetImages()->rowPitch, 0u);

			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Format = textureDesc.Format;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MostDetailedMip = 0;
			srvDesc.Texture2D.MipLevels = -1;
			GetDevice(gfx)->CreateShaderResourceView(pTexture.Get(), &srvDesc, &pTextureView);
			GetContext(gfx)->GenerateMips(pTextureView.Get());
			CUBE_TRACE(std::string("Successfully loaded texture ") + name);
		}
		else
		{
			CUBE_ERROR(std::string("Failed to load a texture ") + name);
		}
	}
}

bool Texture::HasAlpha() const noexcept
{
	return hasAlpha;
}

void Texture::Bind(Graphics& gfx) noexcept
{
	GetContext(gfx)->PSSetShaderResources(slot, 1, pTextureView.GetAddressOf());
}
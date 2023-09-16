#include <sstream>
#include <d3dcompiler.h>
#include "../includes/Graphics.h"
#include "../../CEngine/core/includes/Log.h"
#define GFX_THROW_FAILED(hResultcall) if (FAILED(hResult=(hResultcall))) throw Graphics::GraphicsException(__LINE__, __FILE__, hResult)
#define GFX_DEVICE_REMOVED_EXCEPTION(hResult) Graphics::DeviceRemovedException(__LINE__, __FILE__, (hResult))

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

namespace wrl = Microsoft::WRL;

Graphics::Graphics(HWND hwnd)
{
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = hwnd;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	HRESULT hResult;

	GFX_THROW_FAILED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &sd, &pSwap, &pDevice, nullptr, &pContext));
	wrl::ComPtr<ID3D11Resource> pBackBuffer;
	GFX_THROW_FAILED(pSwap->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer));
	GFX_THROW_FAILED(pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pTarget));
	CUBE_CORE_INFO("D3D was initialized.");
}



void Graphics::ClearBuffer(float red, float green, float blue)
{
	const float color[] = { red, green, blue, 1.0f };
	pContext->ClearRenderTargetView(pTarget.Get(), color);
}

void Graphics::EndFrame()
{
	HRESULT hResult;
	if (FAILED(hResult = pSwap->Present(1u, 0u)))
	{
		if (hResult == DXGI_ERROR_DEVICE_REMOVED)
		{
			throw GFX_DEVICE_REMOVED_EXCEPTION(pDevice->GetDeviceRemovedReason());
		}
		else
		{
			GFX_THROW_FAILED(hResult);
		}
	}
}


void Graphics::DrawTestTriangle()
{

	struct Vertex
	{
		float x;
		float y;
	};

	const Vertex vertices[] = { {0.0f, 0.5f}, {0.5f, -0.5f}, {-0.5f,-0.5f} };

	wrl::ComPtr<ID3D11Buffer> pVertexBuffer;

	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0u;
	bd.MiscFlags = 0u;
	bd.ByteWidth = sizeof(vertices);
	bd.StructureByteStride = sizeof(Vertex);

	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = vertices;
	pDevice->CreateBuffer(&bd, &sd, &pVertexBuffer);

	const UINT stride = sizeof(Vertex);
	const UINT offset = 0u;
	pContext->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);

	wrl::ComPtr<ID3D11PixelShader> pPixelShader;
	wrl::ComPtr<ID3DBlob> pBlob;
	D3DReadFileToBlob(L"PixelShader.cso", &pBlob);
	pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader);
	pContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);

	wrl::ComPtr<ID3D11VertexShader> pVertexShader;
	D3DReadFileToBlob(L"VertexShader.cso", &pBlob);
	pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader);
	pContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);
	

	wrl::ComPtr<ID3D11InputLayout> pInputLayout;
	const D3D11_INPUT_ELEMENT_DESC ied[] = { {"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0} };
	pDevice->CreateInputLayout(ied, (UINT)std::size(ied), pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pInputLayout);
	pContext->IASetInputLayout(pInputLayout.Get());

	pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), nullptr);

	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D11_VIEWPORT vp = {};
	vp.Width = 1280;
	vp.Height = 720;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	pContext->RSSetViewports(1u, &vp);

	pContext->Draw((UINT)std::size(vertices), 0u);
}

Graphics::GraphicsException::GraphicsException(int line, const char* file, HRESULT hResult) : CubeException(line, file), hResult(hResult)
{

}

const char* Graphics::GraphicsException::whatEx() const
{
	std::ostringstream oss;
	oss << getType() << std::endl << "[Error code] 0x" << std::hex << std::uppercase << getErrorCode() << std::dec << " (" << (unsigned long)getErrorCode() << ")" << 
						std::endl << "[Error String] " << getErrorString() << std::endl <<
						std::endl << getOriginString();
	whatExBuffer = oss.str();
	CUBE_CORE_ERROR("Cube Exception was thrown -> \n{} {}", whatExBuffer.c_str(), getType());
	return whatExBuffer.c_str();
}

const char* Graphics::GraphicsException::getType() const
{
	return "Cube Graphics Exception";
}

HRESULT Graphics::GraphicsException::getErrorCode() const
{
	return hResult;
}

std::string Graphics::GraphicsException::getErrorString() const
{
	char* pMessageBuffer = nullptr;
	DWORD nMessageLen = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, hResult, MAKELANGID(LANG_NEPALI, SUBLANG_DEFAULT), reinterpret_cast<wchar_t*>(&pMessageBuffer), 0, nullptr);
	if (nMessageLen == 0)
	{
		return "Unidentified error code.";
	}
	std::string errorString = pMessageBuffer;
	LocalFree(pMessageBuffer);
	return errorString;
}

const char* Graphics::DeviceRemovedException::getType() const
{
	return "Cube Graphics Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}

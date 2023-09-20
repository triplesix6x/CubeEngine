#include <sstream>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "../includes/Graphics.h"
#include "../../includes/Log.h"
#include "../imgui/imgui_impl_dx11.h"
#include "../imgui/imgui_impl_win32.h"
#define GFX_THROW_FAILED(hResultcall) if (FAILED(hResult=(hResultcall))) throw Graphics::GraphicsException(__LINE__, __FILE__, hResult)
#define GFX_DEVICE_REMOVED_EXCEPTION(hResult) Graphics::DeviceRemovedException(__LINE__, __FILE__, (hResult))

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

namespace wrl = Microsoft::WRL;
namespace dx = DirectX;

Graphics::Graphics(HWND hwnd, int width, int height)
{
	//Дексриптор свап чейна DirectX
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_CENTERED;
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

	//Создание девайса и свап чейна
	GFX_THROW_FAILED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &sd, &pSwap, &pDevice, nullptr, &pContext));

	//Задание целевого вида 
	wrl::ComPtr<ID3D11Resource> pBackBuffer;
	GFX_THROW_FAILED(pSwap->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer));
	GFX_THROW_FAILED(pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pTarget));

	//Дескриптор для инициализации буфера оси Z(Глубина)
	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	wrl::ComPtr<ID3D11DepthStencilState> pDSState;
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	pDevice->CreateDepthStencilState(&dsDesc, &pDSState);
	pContext->OMSetDepthStencilState(pDSState.Get(), 1u);

	wrl::ComPtr<ID3D11Texture2D> pDepthStencil;
	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width = (UINT)width;
	descDepth.Height = (UINT)height;
	descDepth.MipLevels = 1u;
	descDepth.ArraySize = 1u;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1u;
	descDepth.SampleDesc.Quality = 0u;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	pDevice->CreateTexture2D(&descDepth, nullptr, &pDepthStencil);

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0u;

	pDevice->CreateDepthStencilView(pDepthStencil.Get(), &descDSV, &pDSV);
	pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), pDSV.Get());

	//Дескриптор для инифиализации viewport
	D3D11_VIEWPORT vp;
	vp.Width = width ;
	vp.Height = height;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	pContext->RSSetViewports(1u, &vp);
	CUBE_CORE_INFO("D3D was initialized.");

	ImGui_ImplDX11_Init(pDevice.Get(), pContext.Get());
	CUBE_CORE_INFO("ImGui DX11 was initialized.");
}



void Graphics::ClearBuffer(float red, float green, float blue)
{
	if (imguiEnabled)
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}
	//Очистка текущего буфера свап чейна
	const float color[] = { red, green, blue, 1.0f };
	pContext->ClearRenderTargetView(pTarget.Get(), color);
	pContext->ClearDepthStencilView(pDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
}

void Graphics::EndFrame()
{
	if (imguiEnabled)
	{
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	//Замена буфера свап чейна
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


void Graphics::DrawIndexed(UINT count)
{
	//Отрисовка пайплайна 
	pContext->DrawIndexed( count, 0u, 0u);
}

void Graphics::SetProjection(DirectX::FXMMATRIX proj) 
{
	projection = proj;
}

DirectX::XMMATRIX Graphics::GetProjection() const 
{
	return projection;
}

void Graphics::EnableImgui() noexcept
{
	imguiEnabled = true;
}

void Graphics::DisableImgui() noexcept
{
	imguiEnabled = false;
}

void Graphics::SetCamera(DirectX::FXMMATRIX cam) noexcept
{
	camera = cam;
}

bool Graphics::IsImguiEnabled() const noexcept
{
	return imguiEnabled;
}

DirectX::XMMATRIX Graphics::GetCamera() const noexcept
{
	return camera;
}

void Graphics::CleanupRenderTarget() noexcept
{
	if (this) { pTarget->Release(); pTarget = nullptr; }
}
void Graphics::CreateTestViewport() noexcept
{
	D3D11_VIEWPORT vp;
	vp.Width = 100;
	vp.Height = 100;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	pContext->RSSetViewports(1u, &vp);
}

void Graphics::CreateViewport(int x, int y) noexcept
{
	D3D11_VIEWPORT vp;
	vp.Width = x;
	vp.Height = y;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	pContext->RSSetViewports(1u, &vp);
}


//Ниже - обработка графических исключений
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

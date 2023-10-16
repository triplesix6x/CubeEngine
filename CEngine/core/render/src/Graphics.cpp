#include <sstream>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <dxgi.h>
#include <math.h>
#include <WICTextureLoader.h>
#include "../includes/Graphics.h"
#include "../../includes/Log.h"
#include "../../imgui/imgui_impl_dx11.h"
#include "../../imgui/imgui_impl_win32.h"
#include "../../imgui/imgui_internal.h"
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
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;
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
	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width = width;
	descDepth.Height = height;
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
	CreateViewport(width, height, 0, 0);
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
	pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), pDSV.Get());
	pContext->ClearRenderTargetView(pTarget.Get(), color);
	pContext->ClearDepthStencilView(pDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u);
}

Graphics::~Graphics()
{
	ImGui_ImplDX11_Shutdown();
}

void Graphics::EndFrame()
{
	if (imguiEnabled)
	{
		HRESULT hResult;
		ImGui::Render();
		pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), pDSV.Get());
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
	//Замена буфера свап чейна
	HRESULT hResult;
	if (VSYNCenabled)
	{
		hResult = pSwap->Present(1u, 0u);
	}
	else
	{
		hResult = pSwap->Present(0u, 0u);
	}
	if (FAILED(hResult))
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


ImGuiID Graphics::ShowDocksape() noexcept
{
		bool show = true;
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::SetNextWindowBgAlpha(0.0f);
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &show, window_flags);
		ImGui::PopStyleVar(3);
		dockspace_id = ImGui::GetID("MyDockspace");
		ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		static bool first_time = true;
		if (first_time)
		{
			first_time = false;
			ImGui::DockBuilderRemoveNode(dockspace_id);
			ImGui::DockBuilderAddNode(dockspace_id, dockspace_flags | ImGuiDockNodeFlags_DockSpace);
			ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);
			ImGuiID dock_id_down = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 0.1f, nullptr, &dockspace_id);
			ImGuiID dock_id_left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.08f, nullptr, &dockspace_id);
			ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.22f, nullptr, &dockspace_id);
			ImGui::DockBuilderDockWindow("Scene", dock_id_right);
			ImGui::DockBuilderDockWindow("ToolBar", dock_id_left);
			ImGui::DockBuilderFinish(dockspace_id);

		}
		ImGui::End();
		return dockspace_id;
}


void Graphics::Resize(UINT width, UINT height) noexcept
{
	CleanupRenderTarget();
	HRESULT hResult;
	GFX_THROW_FAILED(pSwap->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0));
	SetupRenderTarget();

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

void Graphics::ClearScreen(float factor)
{
	const float color[] = { 1.0f, 0.0f, 0.0f, 0.1f};
	pContext->ClearRenderTargetView(pTarget.Get(), color);
}

void Graphics::DrawGrid(DirectX::XMFLOAT3 camPos) noexcept
{
	std::unique_ptr<DirectX::CommonStates> m_states;
	std::unique_ptr<DirectX::BasicEffect> m_effect;
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_batch;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_raster;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

	CD3D11_RASTERIZER_DESC rastDesc(D3D11_FILL_SOLID, D3D11_CULL_NONE, FALSE,
		D3D11_DEFAULT_DEPTH_BIAS, D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
		D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, TRUE, FALSE, FALSE, FALSE);

	pDevice->CreateRasterizerState(&rastDesc, m_raster.ReleaseAndGetAddressOf());

	m_states = std::make_unique<DirectX::CommonStates>(pDevice.Get());

	m_effect = std::make_unique<DirectX::BasicEffect>(pDevice.Get());
	m_effect->SetVertexColorEnabled(true);

	CreateInputLayoutFromEffect<DirectX::VertexPositionColor>(pDevice.Get(), m_effect.get(), m_inputLayout.ReleaseAndGetAddressOf());
	m_batch = std::make_unique <DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> (pContext.Get());

	pContext->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
	pContext->OMSetDepthStencilState(m_states->DepthRead(), 0);
	pContext->RSSetState(m_raster.Get());
	m_effect->SetView(GetCamera());
	m_effect->SetProjection(GetProjection());

	m_effect->Apply(pContext.Get());
	pContext->IASetInputLayout(m_inputLayout.Get());
	m_batch->Begin();

	DirectX::SimpleMath::Vector3 xaxis(300.f, 0.f, 0.f);
	DirectX::SimpleMath::Vector3 zaxis(0.f, 0.f, 300.f);
	DirectX::SimpleMath::Vector3 origin = DirectX::SimpleMath::Vector3(round(camPos.x - 5.0f), 0.f, round(camPos.z + 10.0f));

	constexpr size_t divisions = 600;

	for (size_t i = 0; i <= divisions; ++i)
	{
		float fPercent = float(i) / float(divisions);
		fPercent = (fPercent * 2.0f) - 1.0f;

		DirectX::SimpleMath::Vector3 scale = xaxis * fPercent + origin;

		DirectX::VertexPositionColor v1(scale - zaxis, DirectX::Colors::SlateGray);
		DirectX::VertexPositionColor v2(scale + zaxis, DirectX::Colors::SlateGray);
		m_batch->DrawLine(v1, v2);
	}

	for (size_t i = 0; i <= divisions; i++)
	{
		float fPercent = float(i) / float(divisions);
		fPercent = (fPercent * 2.0f) - 1.0f;

		DirectX::SimpleMath::Vector3 scale = zaxis * fPercent + origin;

		DirectX::VertexPositionColor v1(scale - xaxis, DirectX::Colors::SlateGray);
		DirectX::VertexPositionColor v2(scale + xaxis, DirectX::Colors::SlateGray);
		m_batch->DrawLine(v1, v2);
	}

	m_batch->End();
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

void Graphics::SetTexture(ID3D11ShaderResourceView** tv, const wchar_t* file)
{
	DirectX::CreateWICTextureFromFile(pDevice.Get(), pContext.Get(), file, nullptr, tv);
}

bool Graphics::IsImguiEnabled() const noexcept
{
	return imguiEnabled;
}

bool Graphics::isVSYCNenabled() const noexcept
{
	return VSYNCenabled;
}

void Graphics::enableVSYNC() noexcept
{
	VSYNCenabled = true;
}

void Graphics::disableVSYNC() noexcept
{
	VSYNCenabled = false;
}

int Graphics::getMonitorFrequency() const noexcept
{
	DEVMODE dm;
	EnumDisplaySettings(NULL, 0, &dm);
	return dm.dmDisplayFrequency;
}


DirectX::XMMATRIX Graphics::GetCamera() const noexcept
{
	return camera;
}


void Graphics::SetupRenderTarget() noexcept
{
	HRESULT hResult;
	wrl::ComPtr<ID3D11Resource> pBackBuffer;
	GFX_THROW_FAILED(pSwap->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer));
	GFX_THROW_FAILED(pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pTarget));
	pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), pDSV.Get());
}


void Graphics::CleanupRenderTarget() noexcept
{
	pContext->OMSetRenderTargets(0, NULL, NULL); 
	pTarget->Release();
}


void Graphics::CreateViewport(float x, float y, float topx, float topy) noexcept
{

	D3D11_VIEWPORT vp;
	vp.Width = x;
	vp.Height = y;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = topx - ImGui::GetMainViewport()->Pos.x;
	vp.TopLeftY = topy - ImGui::GetMainViewport()->Pos.y;
	pContext->RSSetViewports(1u, &vp);

}


//Ниже - обработка графических исключений
Graphics::GraphicsException::GraphicsException(int line, const char* file, HRESULT hResult) : CubeException(line, file), hResult(hResult)
{
	MessageBoxA(nullptr, whatEx(), getType(), MB_OK | MB_ICONEXCLAMATION);
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
	DWORD nMessageLen = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_IGNORE_INSERTS,
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

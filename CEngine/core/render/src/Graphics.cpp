#include <sstream>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <dxgi.h>
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
		ImGui::End();

		return dockspace_id;
}

void Graphics::ShowMenuBar() noexcept
{
	if (ImGui::BeginMainMenuBar()) 
	{
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Create")) 
			{
			}
			if (ImGui::MenuItem("Open")) 
			{
			}
			if (ImGui::MenuItem("Save"))
			{
			}
			if (ImGui::MenuItem("Save as..")) {
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Themes")) {
			if (ImGui::MenuItem("Default Dark"))
			{
				SetDDTheme();
			}
			if (ImGui::MenuItem("Classic Light"))
			{
				SetLightTheme();
			}
			if (ImGui::MenuItem("Deep Purple"))
			{
				SetDPTheme();
			}
			if (ImGui::MenuItem("Sailor moon")) 
			{
				SetSMTheme();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

void Graphics::SetDDTheme() noexcept
{
	ImGui::StyleColorsDark();
	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
	colors[ImGuiCol_Border] = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
	colors[ImGuiCol_Button] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
	colors[ImGuiCol_Separator] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
	colors[ImGuiCol_Tab] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_DockingPreview] = ImVec4(0.69f, 0.69f, 0.69f, 1.00f);
	colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.00f, 0.50f, 1.00f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.00f, 0.50f, 1.00f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.00f, 0.50f, 1.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.00f, 0.50f, 1.00f, 1.00f);
	colors[ImGuiCol_TableHeaderBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_TableBorderStrong] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_TableBorderLight] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
	colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.00f, 0.50f, 1.00f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.00f, 0.50f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.00f, 0.50f, 1.00f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.00f, 0.50f, 1.00f, 0.35f);
}

void Graphics::SetLightTheme() noexcept
{
	ImGui::StyleColorsLight();
	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.94f, 0.94f, 0.94f, 0.94f);
	colors[ImGuiCol_PopupBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
	colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.39f);
	colors[ImGuiCol_BorderShadow] = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
	colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.69f, 0.69f, 0.69f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
}

void Graphics::SetDPTheme() noexcept
{
	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.1f, 0.13f, 1.0f };
	colors[ImGuiCol_MenuBarBg] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
	colors[ImGuiCol_Border] = ImVec4{ 0.44f, 0.37f, 0.61f, 0.29f };
	colors[ImGuiCol_BorderShadow] = ImVec4{ 0.0f, 0.0f, 0.0f, 0.24f };
	colors[ImGuiCol_Text] = ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f };
	colors[ImGuiCol_TextDisabled] = ImVec4{ 0.5f, 0.5f, 0.5f, 1.0f };
	colors[ImGuiCol_Header] = ImVec4{ 0.13f, 0.13f, 0.17, 1.0f };
	colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.19f, 0.2f, 0.25f, 1.0f };
	colors[ImGuiCol_HeaderActive] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
	colors[ImGuiCol_Button] = ImVec4{ 0.13f, 0.13f, 0.17, 1.0f };
	colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.19f, 0.2f, 0.25f, 1.0f };
	colors[ImGuiCol_ButtonActive] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
	colors[ImGuiCol_CheckMark] = ImVec4{ 0.74f, 0.58f, 0.98f, 1.0f };
	colors[ImGuiCol_PopupBg] = ImVec4{ 0.1f, 0.1f, 0.13f, 0.92f };
	colors[ImGuiCol_SliderGrab] = ImVec4{ 0.44f, 0.37f, 0.61f, 0.54f };
	colors[ImGuiCol_SliderGrabActive] = ImVec4{ 0.74f, 0.58f, 0.98f, 0.54f };
	colors[ImGuiCol_FrameBg] = ImVec4{ 0.13f, 0.13, 0.17, 1.0f };
	colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.19f, 0.2f, 0.25f, 1.0f };
	colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
	colors[ImGuiCol_Tab] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
	colors[ImGuiCol_TabHovered] = ImVec4{ 0.24, 0.24f, 0.32f, 1.0f };
	colors[ImGuiCol_TabActive] = ImVec4{ 0.2f, 0.22f, 0.27f, 1.0f };
	colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
	colors[ImGuiCol_TitleBg] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
	colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
	colors[ImGuiCol_ScrollbarBg] = ImVec4{ 0.1f, 0.1f, 0.13f, 1.0f };
	colors[ImGuiCol_ScrollbarGrab] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4{ 0.19f, 0.2f, 0.25f, 1.0f };
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4{ 0.24f, 0.24f, 0.32f, 1.0f };
	colors[ImGuiCol_Separator] = ImVec4{ 0.44f, 0.37f, 0.61f, 1.0f };
	colors[ImGuiCol_SeparatorHovered] = ImVec4{ 0.74f, 0.58f, 0.98f, 1.0f };
	colors[ImGuiCol_SeparatorActive] = ImVec4{ 0.84f, 0.58f, 1.0f, 1.0f };
	colors[ImGuiCol_ResizeGrip] = ImVec4{ 0.44f, 0.37f, 0.61f, 0.29f };
	colors[ImGuiCol_ResizeGripHovered] = ImVec4{ 0.74f, 0.58f, 0.98f, 0.29f };
	colors[ImGuiCol_ResizeGripActive] = ImVec4{ 0.84f, 0.58f, 1.0f, 0.29f };
	colors[ImGuiCol_DockingPreview] = ImVec4{ 0.44f, 0.37f, 0.61f, 1.0f };
}

void Graphics::SetSMTheme() noexcept
{
	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = ImVec4(0.01, 0.36, 1.00, 1.00);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.00, 0.60, 0.67, 0.97);
	colors[ImGuiCol_WindowBg] = ImVec4(0.02, 0.00, 0.06, 1.00);
	colors[ImGuiCol_PopupBg] = ImVec4(0.00, 0.00, 0.00, 1.00);
	colors[ImGuiCol_Border] = ImVec4(0.07, 0.10, 0.15, 0.56);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00, 0.00, 0.00, 0.49);
	colors[ImGuiCol_FrameBg] = ImVec4(0.06, 0.19, 0.46, 0.29);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.03, 0.00, 0.06, 0.22);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.00, 0.00, 0.00, 0.10);
	colors[ImGuiCol_TitleBg] = ImVec4(0.01, 0.01, 0.05, 1.00);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.14, 0.26, 0.55, 1.00);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.40, 0.40, 0.90, 0.20);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.00, 0.00, 0.00, 0.80);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.27, 0.00, 1.00, 0.19);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.00, 1.00, 0.95, 0.30);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.00, 0.00, 0.00, 0.40);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.02, 0.98, 1.00, 0.40);
	colors[ImGuiCol_CheckMark] = ImVec4(0.00, 0.58, 1.00, 1.00);
	colors[ImGuiCol_SliderGrab] = ImVec4(1.00, 1.00, 1.00, 0.30);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.80, 0.50, 0.50, 1.00);
	colors[ImGuiCol_Button] = ImVec4(0.09, 0.06, 0.20, 1.00);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.08, 0.03, 0.21, 0.27);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.00, 0.54, 1.00, 1.00);
	colors[ImGuiCol_Header] = ImVec4(0.35, 0.02, 1.00, 0.45);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.06, 0.39, 0.40, 0.80);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.00, 0.86, 1.00, 0.80);
	colors[ImGuiCol_Separator] = ImVec4(0.07, 0.30, 0.52, 1.00);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.00, 0.00, 0.00, 1.00);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.06, 0.06, 0.90, 1.00);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.02, 0.01, 0.27, 0.30);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.24, 0.00, 0.87, 0.60);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.00, 0.00, 0.00, 0.90);
	colors[ImGuiCol_PlotLines] = ImVec4(0.45, 0.00, 0.73, 1.00);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.07, 0.02, 0.39, 1.00);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.06, 0.05, 0.12, 1.00);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.10, 0.06, 0.27, 1.00);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.17, 0.06, 0.41, 0.35);
	colors[ImGuiCol_DockingPreview] = ImVec4{ 0.14f, 0.37f, 0.61f, 1.0f };
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

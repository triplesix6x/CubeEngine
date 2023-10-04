#pragma once
#include <windows.h>
#include <string>
#include "DXTKIncludes.h"
#include <wrl.h>
#include "../../../exception/includes/CubeException.h"
#include <d3d11.h>
#include <memory>
#include <d3dcompiler.h>
#include <random>
#include "../../imgui/imgui_internal.h"
#include <DirectXMath.h>

namespace wrl = Microsoft::WRL;

class Graphics
{
	friend class Bindable;
	friend class Texture;
	friend class SkyBoxTex;
public:
	class GraphicsException : public CubeException
	{
	using CubeException::CubeException;
	public:
		GraphicsException(int line, const char* file, HRESULT hResult);
		const char* whatEx() const;
		const char* getType() const;
		HRESULT getErrorCode() const;
		std::string getErrorString() const;
	private:
		HRESULT hResult;
	};
	class DeviceRemovedException : public GraphicsException
	{
		using GraphicsException::GraphicsException;
	public:
		const char* getType() const;
	};
public:
	Graphics(HWND hwnd, int width, int height);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics();
	void EndFrame();
	void ClearBuffer(float red, float green, float blue);
	void DrawIndexed(UINT count);
	void SetProjection(DirectX::FXMMATRIX proj);
	void SetCamera(DirectX::FXMMATRIX cam) noexcept;
	void SetTexture(ID3D11ShaderResourceView** tv, const wchar_t* file);
	DirectX::XMMATRIX GetCamera() const noexcept;
	DirectX::XMMATRIX GetProjection() const;

	void DrawGrid(DirectX::XMFLOAT3 camPos) noexcept;

	ImGuiID ShowDocksape() noexcept;
	void SetupRenderTarget() noexcept;
	void CleanupRenderTarget() noexcept;
	void CreateViewport(float x, float y, float topx, float topy) noexcept;
	void EnableImgui() noexcept;
	void DisableImgui() noexcept;
	void Resize(UINT width, UINT height) noexcept;
	bool IsImguiEnabled() const noexcept;
private:
	bool imguiEnabled = true;
	DirectX::XMMATRIX projection;
	DirectX::XMMATRIX camera;
	ImGuiID dockspace_id;
	wrl::ComPtr<ID3D11Device> pDevice = nullptr;
	wrl::ComPtr<IDXGISwapChain> pSwap = nullptr;
	wrl::ComPtr<ID3D11DeviceContext> pContext;
	wrl::ComPtr<ID3D11RenderTargetView> pTarget;
	wrl::ComPtr<ID3D11DepthStencilView> pDSV;
	wrl::ComPtr<ID3D11Texture2D> pDepthStencil;
};
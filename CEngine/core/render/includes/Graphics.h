#pragma once
#include <windows.h>
#include <string>
#include <wrl.h>
#include "../exception/includes/CubeException.h"
#include <d3d11.h>

namespace wrl = Microsoft::WRL;

class Graphics
{
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
	Graphics(HWND hwnd);
	~Graphics() = default;
	void EndFrame();
	void ClearBuffer(float red, float green, float blue);
	void DrawTestTriangle();
private:
	wrl::ComPtr<ID3D11Device> pDevice = nullptr;
	wrl::ComPtr<IDXGISwapChain> pSwap;
	wrl::ComPtr<ID3D11DeviceContext> pContext;
	wrl::ComPtr<ID3D11RenderTargetView> pTarget;
};
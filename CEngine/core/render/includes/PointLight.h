#pragma once
#include "Graphics.h"
#include "SolidSphere.h"
#include "ConstantBuffers.h"


struct PointLightCBuf
{
	alignas(16) DirectX::XMFLOAT3 pos;
	alignas(16) DirectX::XMFLOAT3 ambient;
	alignas(16) DirectX::XMFLOAT3 diffuseColor;
	float diffuseIntensity;
    float attConst;
	float attLin;
	float attQuad;
};

class Lights
{
public:
	class PointLight
	{
	public:
		PointLight(Graphics& gfx, int id, float radius = 0.5f);
		void SpawnControlWindow() noexcept;
		void Reset() noexcept;
		void Draw(Graphics& gfx) const noexcept;
		bool DrawSphere(); 
		void Bind(Graphics& gfx, DirectX::FXMMATRIX view) const noexcept;
		PointLightCBuf getCbuf() const;
		int id;
	private:
		bool drawSphere = true;
		PointLightCBuf cbData;
		mutable SolidSphere mesh;
	};
	Lights(Graphics& gfx, float radius = 0.5f);
	void AddLight(Graphics& gfx);
	void DeleteLight(int i);
	void UpdateCbufs();
	void Bind(Graphics& gfx, DirectX::FXMMATRIX view) noexcept;
	PointLightCBuf cbufs[32];
	std::vector<std::unique_ptr<PointLight>> sceneLights;
private:
	int id = 0;
	mutable PixelConstantBuffer<PointLightCBuf> cbuf;
};
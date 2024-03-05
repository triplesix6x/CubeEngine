//Класс для настройки и взаимодействия с источником освещения. Хранит в себе все источники освещения
//И позволяет управлять ими

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
		PointLight(Graphics& gfx, int id, float radius, std::string lightName);
		void SpawnControlWindow() noexcept;
		void Reset() noexcept;
		void Submit(class FrameCommander& frame) const noexcept;
		bool DrawSphere(); 
		PointLightCBuf getCbuf() const;
		void setCbuf(PointLightCBuf Cbuf);
		const int id;
		std::string lightName;
		bool drawSphere = true;
	private:
		PointLightCBuf cbData;
		mutable SolidSphere mesh;
	};
	Lights(Graphics& gfx, float radius = 0.5f);
	void AddLight(Graphics& gfx,std::string lightName = "Unnamed Light");
	int getLighstCount();
	void clearLights();
	int getId(int i);
	std::string getName(int i);
	void DeleteLight(int i);
	void UpdateCbufs();
	void spawnWnds();
	void drawSpheres(class FrameCommander& frame);
	void Bind(Graphics& gfx, DirectX::FXMMATRIX view) noexcept;
	std::vector<std::unique_ptr<PointLight>> sceneLights;
private:
	int id = 0;
	PointLightCBuf cbufs[32];
	mutable PixelConstantBuffer<PointLightCBuf> cbuf;
};
#pragma once
#include "DrawableBase.h"

class TestPlane : public DrawableBase<TestPlane>
{
public:
	TestPlane(Graphics& gfx, float size);
	void SetPos(DirectX::XMFLOAT3 pos) noexcept;
	void SetRotation(float roll, float pitch, float yaw) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	void spawnControlWindow(Graphics& gfx);
private:
	struct PSMaterialConstant
	{
		float specularIntensity = 0.18f;
		float specularPower = 18.0f;
		BOOL normalMapEnabled = TRUE;
		float padding[1];
	} pmc;
	DirectX::XMFLOAT3 pos = { 0.0f,0.0f,0.0f };
	float roll = 0.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;
};
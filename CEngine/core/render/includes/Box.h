#pragma once
#include "DrawableBase.h"

class Box : public DrawableBase<Box>
{
public:
	Box(Graphics& gfx, std::mt19937& rng,
		std::uniform_real_distribution<float>& adist,
		std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist,
		std::uniform_real_distribution<float>& rdist,
		std::uniform_real_distribution<float>& bdist);
	void Update(float dt) noexcept override;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	//Позиция в пространстве
	float r;				//Радиус от центра
	float roll = 0.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;
	float theta;
	float phi;
	float chi;
	//Скорость дельта/с
	float droll;			//Скорости вращение вокруг своих осей
	float dpitch;
	float dyaw;
	float dtheta;			//Скорости вращения вокруг центра
	float dphi;
	float dchi;

	DirectX::XMFLOAT3X3 mt;
};
//Класс для определения и добавления в пайплайн трафарета глубины DirectX (Bindable)

#pragma once
#include "Bindable.h"

class DepthStencil : public Bindable
{
public:
	DepthStencil(Graphics& gfx, bool skybox);
	void Bind(Graphics& gfx)  noexcept override;
protected:
	wrl::ComPtr <ID3D11DepthStencilState> pDss;
};
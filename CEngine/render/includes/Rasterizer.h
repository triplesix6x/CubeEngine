//Класс для определения и добавления в пайплайн растеризатора DirectX (Bindable)

#pragma once
#include "Bindable.h"



	class Rasterizer : public Bindable
	{
	public:
		Rasterizer(Graphics& gfx, bool twoSided);
		void Bind(Graphics& gfx)  noexcept override;
	protected:
		wrl::ComPtr <ID3D11RasterizerState> RSCull;
	};

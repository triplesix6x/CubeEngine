//Класс для определения и добавления в пайплайн трафарета глубины DirectX (Bindable)

#pragma once
#include "Bindable.h"

	class DepthStencil : public Bindable
	{
	public:
		enum class Mode
		{
			Off,
			Write,
			Mask,
			skybox
		};
		DepthStencil(Graphics& gfx, Mode mode);
		void Bind(Graphics& gfx)  noexcept override;
	protected:
		wrl::ComPtr <ID3D11DepthStencilState> pDss;
	private:
		Mode mode;
	};

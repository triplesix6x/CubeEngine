//Класс для определения и добавления в пайплайн входного шаблона DirectX (Bindable)

#pragma once
#include "Bindable.h"
#include "CVertex.h"


	class InputLayout : public Bindable
	{
	public:
		InputLayout(Graphics& gfx,
			CubeR::VertexLayout layout,
			ID3DBlob* pVertexShaderBytecode);
		void Bind(Graphics& gfx)  noexcept override;
		const CubeR::VertexLayout GetLayout() const noexcept;
	protected:
		CubeR::VertexLayout layout;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
	};

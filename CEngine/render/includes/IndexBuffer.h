//Класс для определения и добавления в пайплайн буфера индексов DirectX (Bindable)

#pragma once
#include "Bindable.h"

	class IndexBuffer : public Bindable
	{
	public:
		IndexBuffer(Graphics& gfx, const std::vector<unsigned short>& indices) noexcept;
		void Bind(Graphics& gfx) noexcept override;
		UINT GetCount() const;
	protected:
		UINT count;
		Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
	};

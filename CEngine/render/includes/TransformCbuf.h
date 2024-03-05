//Класс для определения и добавления в пайплайн константного буфера DirectX (Bindable)


#pragma once
#include "ConstantBuffers.h"
#include "Drawable.h"
#include <DirectXMath.h>


	class TransformCbuf : public Bindable
	{
	protected:
		struct Transforms
		{
			DirectX::XMMATRIX modelview;
			DirectX::XMMATRIX modelViewProj;
		};
	public:
		TransformCbuf(Graphics& gfx, UINT slot = 0u);
		void Bind(Graphics& gfx)  noexcept override;
		void InitializeParentReference(const Drawable& parent) noexcept override;
	protected:
		void UpdateBindImpl(Graphics& gfx, const Transforms& tf);
		Transforms GetTransforms(Graphics& gfx);
	private:
		static std::unique_ptr<VertexConstantBuffer<Transforms>> pVcbuf;
		const Drawable* pParent = nullptr;
	};
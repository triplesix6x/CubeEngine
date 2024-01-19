//Класс для определения и добавления в пайплайн топологии вершин и индексов DirectX (Bindable)


#pragma once
#include "Bindable.h"


	class Topology : public Bindable
	{
	public:
		Topology(Graphics& gfx, D3D11_PRIMITIVE_TOPOLOGY type);
		void Bind(Graphics& gfx)  noexcept override;
	protected:
		D3D11_PRIMITIVE_TOPOLOGY type;
	};

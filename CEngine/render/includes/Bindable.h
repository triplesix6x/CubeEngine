//Родительский класс для всех объектов, которые можно добавить в пайплайн

#pragma once
#include "Graphics.h"

class Bindable
{
public:
	//Функция Bind переопределяется в каждом дочернем классе Bindable 
	// и отвечает за добавление объекта в пайплайн DirectX
	virtual void Bind(Graphics& gfx) noexcept = 0;
	virtual ~Bindable() = default;
protected:
	static ID3D11DeviceContext* GetContext(Graphics& gfx);
	static ID3D11Device* GetDevice(Graphics& gfx);
};
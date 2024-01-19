//Родительский класс для всех объектов, которые отрисовываются на экране
//В нем хрантся все Bindable элементы для конкретного трехмерного объекта а также осуществляется
// обновление параметров объекта

#pragma once
#include "Graphics.h"
#include "Technique.h"
#include <DirectXMath.h>


class IndexBuffer;
class VertexBuffer;
class Topology;
class InputLayout;


class Drawable
{
	template<class T>
	friend class DrawableBase;
public:
	Drawable() = default;
	Drawable(const Drawable&) = delete;
	void AddTechnique(Technique tech_in) noexcept;
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	void Submit(class FrameCommander& frame) const noexcept;
	void Bind(Graphics& gfx) const noexcept;
	UINT GetIndexCount() const noexcept;
	virtual ~Drawable();
protected:
	std::shared_ptr<IndexBuffer> pIndices; 
	std::shared_ptr<VertexBuffer> pVertices; 
	std::shared_ptr<Topology> pTopology; 
	std::vector<Technique> techniques;
}; 
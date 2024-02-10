//Родительский класс для всех объектов, которые отрисовываются на экране
//В нем хрантся все Bindable элементы для конкретного трехмерного объекта а также осуществляется
// обновление параметров объекта

#pragma once
#include <DirectXMath.h>
#include "Technique.h"
#include "Graphics.h"


class Material;
struct aiMesh;


class IndexBuffer;
class VertexBuffer;
class Topology;
class InputLayout;

class Drawable
{
public:
	Drawable() = default;
	Drawable(Graphics& gfx, const Material& mat, const aiMesh& mesh) noexcept;
	Drawable(const Drawable&) = delete;
	void AddTechnique(Technique tech_in) noexcept;
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	void Submit(class FrameCommander& frame) const noexcept;
	void Bind(Graphics& gfx) const noexcept;
	UINT GetIndexCount() const noexcept;
	virtual ~Drawable();
protected:
	std::unique_ptr<IndexBuffer> pIndices; 
	std::unique_ptr<VertexBuffer> pVertices;
	std::unique_ptr<Topology> pTopology;
	std::vector<Technique> techniques;
}; 
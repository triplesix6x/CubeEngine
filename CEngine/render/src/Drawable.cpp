#include "../includes/Drawable.h"
#include "../includes/IndexBuffer.h"
#include "../includes/BindableBase.h"
#include "../includes/Material.h"
#include <cassert>
#include <typeinfo>


Drawable::Drawable(Graphics& gfx, const Material& mat, const aiMesh& mesh) noexcept
{
	pVertices = mat.MakeVertexBindable(gfx, mesh);
	pIndices = mat.MakeIndexBindable(gfx, mesh);
	pTopology = std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (auto& t : mat.GetTechniques())
	{
		AddTechnique(std::move(t));
	}
}

void Drawable::Submit(FrameCommander& frame) const noexcept
{
	for (const auto& tech : techniques)
	{
		tech.Submit(frame, *this);
	}
}

void Drawable::AddTechnique(Technique tech_in) noexcept
{
	tech_in.InitializeParentReferences(*this);
	techniques.push_back(std::move(tech_in));
}

void Drawable::Bind(Graphics& gfx) const noexcept
{
	pTopology->Bind(gfx);
	pIndices->Bind(gfx);
	pVertices->Bind(gfx);
}

UINT Drawable::GetIndexCount() const noexcept
{
	return pIndices->GetCount();
}

Drawable::~Drawable()
{}
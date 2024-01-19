#include "../includes/Drawable.h"
#include "../includes/IndexBuffer.h"
#include "../includes/BindableBase.h"
#include <cassert>
#include <typeinfo>


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
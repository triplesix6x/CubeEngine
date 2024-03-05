#include "../includes/Step.h"
#include "../includes/Drawable.h"
#include "../includes/FrameCommander.h"

void Step::Submit(FrameCommander& frame, const Drawable& drawable) const
{
	frame.Accept(Job{ this,&drawable }, targetPass);
}

void Step::InitializeParentReferences(const Drawable& parent) noexcept
{
	for (auto& b : bindables)
	{
		b->InitializeParentReference(parent);
	}
}
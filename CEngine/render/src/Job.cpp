#include "../includes/Job.h"
#include "../includes/Step.h"
#include "../includes/Drawable.h"


Job::Job(const Step* pStep, const Drawable* pDrawable)
	:
	pDrawable{ pDrawable },
	pStep{ pStep }
{}

void Job::Execute(Graphics& gfx) const noexcept
{
	pDrawable->Bind(gfx);
	pStep->Bind(gfx);
	gfx.DrawIndexed(pDrawable->GetIndexCount());
}
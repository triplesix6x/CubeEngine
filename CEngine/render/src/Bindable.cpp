#include "../includes/Bindable.h"

ID3D11DeviceContext* Bindable::GetContext(Graphics& gfx)
{
	return gfx.pContext.Get();
}

ID3D11Device* Bindable::GetDevice(Graphics& gfx)
{
	return gfx.pDevice.Get();
}

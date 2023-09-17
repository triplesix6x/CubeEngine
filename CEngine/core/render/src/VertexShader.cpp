#include "../includes/VertexShader.h"


VertexShader::VertexShader(Graphics& gfx, const std::wstring& path)
{

	D3DReadFileToBlob(path.c_str(), &pBytecodeBlob);
	GetDevice(gfx)->CreateVertexShader(
		pBytecodeBlob->GetBufferPointer(),
		pBytecodeBlob->GetBufferSize(),
		nullptr,
		&pVertexShader
	);
}

void VertexShader::Bind(Graphics& gfx)  noexcept
{
	GetContext(gfx)->VSSetShader(pVertexShader.Get(), nullptr, 0u);
}

ID3DBlob* VertexShader::GetBytecode() const 
{
	return pBytecodeBlob.Get();
}
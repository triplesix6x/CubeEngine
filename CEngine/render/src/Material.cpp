#include "../includes/Material.h"


Material::Material(Graphics& gfx, const aiMaterial& material, const std::filesystem::path& path)
	:
modelPath(path.string())
{
	namespace dx = DirectX;
	using CubeR::VertexLayout;
	std::string dir = path.parent_path().string() + "\\";
	 
	aiString tempName; 
	material.Get(AI_MATKEY_NAME, tempName); 
	name = tempName.C_Str(); 

	std::wstring shaderCode = L"Phong";

	Technique standard;
	Step Phong(1);
	
	using namespace std::string_literals;


	float shine = 2.0f;
	dx::XMFLOAT4 specularColor = { 0.18f, 0.18f, 0.18f, 1.0f };
	dx::XMFLOAT4 diffuseColor = { 0.45f, 0.45f, 0.85f, 1.0f };
	aiString texFileName;

	vtxLayout.Append(CubeR::VertexLayout::Position3D); 
	vtxLayout.Append(CubeR::VertexLayout::Normal);

	if (material.GetTexture(aiTextureType_SPECULAR, 0, &texFileName) == aiReturn_SUCCESS)
	{
		auto tex = std::make_unique<Texture>(gfx, dir + texFileName.C_Str(), 1);
		hasAlphaGloss = tex->HasAlpha();
		Phong.AddBindable(std::move(tex));
		hasSpecMap = true;
		shaderCode += L"Spec";
	}
	else
	{
		material.Get(AI_MATKEY_COLOR_SPECULAR, reinterpret_cast<aiColor3D&>(specularColor));
	}
	if (!hasAlphaGloss)
	{
		material.Get(AI_MATKEY_SHININESS, shine);
	}
	if (material.GetTexture(aiTextureType_NORMALS, 0, &texFileName) == aiReturn_SUCCESS)
	{
		vtxLayout.Append(CubeR::VertexLayout::Tangent); 
		vtxLayout.Append(CubeR::VertexLayout::Bitangent); 
		auto tex = std::make_unique<Texture>(gfx, dir + texFileName.C_Str(), 2);
		hasAlphaGloss = tex->HasAlpha();
		Phong.AddBindable(std::move(tex));
		hasNormalMap = true;
		shaderCode += L"NormalMap";
	}
	if (material.GetTexture(aiTextureType_DIFFUSE, 0, &texFileName) == aiReturn_SUCCESS) 
	{
		vtxLayout.Append(CubeR::VertexLayout::Texture2D);  
		auto tex = std::make_unique<Texture>(gfx, dir + texFileName.C_Str()); 
		Phong.AddBindable(std::move(tex)); 
		hasDiffuseMap = true; 
	}
	else
	{
		shaderCode = L"NoTexPhong"; 
		material.Get(AI_MATKEY_COLOR_DIFFUSE, reinterpret_cast<aiColor3D&>(diffuseColor)); 
	} 
	if (hasDiffuseMap || hasNormalMap || hasSpecMap)
	{
		Phong.AddBindable(std::make_unique<Sampler>(gfx));
	}
	if (hasDiffuseMap && hasNormalMap && hasSpecMap)
	{
		struct PSMaterialConstantFullmonte
		{
			BOOL  normalMapEnabled = TRUE;
			BOOL specularMapEnabled = TRUE;
			BOOL  hasGlossMap = FALSE;
			float specularPower = 3.1f;
			DirectX::XMFLOAT3 specularColor = { 0.75f,0.75f,0.75f };
			float specularMapWeight = 0.671f;
		} pmc;
		pmc.specularPower = shine;
		pmc.hasGlossMap = hasAlphaGloss ? TRUE : FALSE;
		Phong.AddBindable(std::make_unique<PixelConstantBuffer<PSMaterialConstantFullmonte>>(gfx, pmc, 1u));
	}

	else if (hasDiffuseMap && !hasNormalMap && hasSpecMap)
	{
		struct PSMaterialConstantDissSpec {
			float specularMapWeight;
			BOOL hasGloss;
			float specularConstPow;
			float padding;
		} pmc;
		pmc.specularConstPow = shine; 
		pmc.hasGloss = hasAlphaGloss ? TRUE : FALSE; 
		pmc.specularMapWeight = 1.0f; 
		Phong.AddBindable(std::make_unique<PixelConstantBuffer<PSMaterialConstantDissSpec>>(gfx, pmc, 1u));
	}

	else if (hasDiffuseMap && hasNormalMap)
	{
		struct PSMaterialConstantDiffnorm
		{
			float specularIntensity = 0.18f;
			float specularPower = 2.0f;
			BOOL  normalMapEnabled = TRUE;
			float padding[1];
		} pmc;
		Phong.AddBindable(std::make_unique<PixelConstantBuffer<PSMaterialConstantDiffnorm>>(gfx, pmc, 1u));
	}

	else if (hasDiffuseMap)
	{
		struct PSMaterialConstantDiffuse
		{
			float specularIntensity = 0.18f;
			float specularPower = 2.0f;
			float padding[2];
		} pmc;
		Phong.AddBindable(std::make_unique<PixelConstantBuffer<PSMaterialConstantDiffuse>>(gfx, pmc, 1u));
	}

	else if (!hasDiffuseMap && !hasNormalMap && !hasSpecMap)
	{
		struct PSMaterialConstantNotex
		{
			DirectX::XMFLOAT4 color;
			float specularIntensity = 0.65f;
			float specularPower = 2.0f;
			float padding[2];
		} pmc;
		pmc.color = diffuseColor;
		pmc.specularIntensity = (specularColor.x + specularColor.y + specularColor.z) / 3.0f;
		Phong.AddBindable(std::make_unique<PixelConstantBuffer<PSMaterialConstantNotex>>(gfx, pmc, 1u));
	}
	else
	{
		throw std::runtime_error("Wrong texture combination");
	}
	auto pvs = std::make_unique<VertexShader>(gfx, L"shaders\\" + shaderCode + L"VS.cso");
	auto pvsbc = pvs->GetBytecode();
	Phong.AddBindable(std::move(pvs));
	Phong.AddBindable(std::make_unique<PixelShader>(gfx, L"shaders\\" + shaderCode + L"PS.cso"));
	Phong.AddBindable(std::make_unique<TransformCbuf>(gfx));
	Phong.AddBindable(std::make_unique<InputLayout>(gfx, vtxLayout, pvsbc));

	standard.AddStep(Phong);
	techniques.push_back(standard);
}


CubeR::VertexBuffer Material::ExtractVertices(const aiMesh& mesh) const noexcept
{
	CubeR::VertexBuffer vbuf(vtxLayout); 
	if (hasDiffuseMap && hasNormalMap && hasSpecMap)
	{
		for (unsigned int i = 0; i < mesh.mNumVertices; i++)
		{
			vbuf.EmplaceBack(
				DirectX::XMFLOAT3(mesh.mVertices[i].x * 1.0f, mesh.mVertices[i].y * 1.0f, mesh.mVertices[i].z * 1.0f),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mNormals[i]),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mTangents[i]),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mBitangents[i]),
				*reinterpret_cast<DirectX::XMFLOAT2*>(&mesh.mTextureCoords[0][i])
			);
		}
	}
	else if (hasDiffuseMap && !hasNormalMap && hasSpecMap)
	{
		for (unsigned int i = 0; i < mesh.mNumVertices; i++)
		{
			vbuf.EmplaceBack(
				DirectX::XMFLOAT3(mesh.mVertices[i].x * 1.0f, mesh.mVertices[i].y * 1.0f, mesh.mVertices[i].z * 1.0f),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mNormals[i]),
				*reinterpret_cast<DirectX::XMFLOAT2*>(&mesh.mTextureCoords[0][i])
			);
		}
	}
	else if (hasDiffuseMap && hasNormalMap)
	{
		for (unsigned int i = 0; i < mesh.mNumVertices; i++)
		{
			vbuf.EmplaceBack(
				DirectX::XMFLOAT3(mesh.mVertices[i].x * 1.0f, mesh.mVertices[i].y * 1.0f, mesh.mVertices[i].z * 1.0f),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mNormals[i]),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mTangents[i]),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mBitangents[i]),
				*reinterpret_cast<DirectX::XMFLOAT2*>(&mesh.mTextureCoords[0][i])
			);
		}
	}
	else if (hasDiffuseMap)
	{
		for (unsigned int i = 0; i < mesh.mNumVertices; i++)
		{
			vbuf.EmplaceBack(
				DirectX::XMFLOAT3(mesh.mVertices[i].x * 1.0f, mesh.mVertices[i].y * 1.0f, mesh.mVertices[i].z * 1.0f),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mNormals[i]),
				*reinterpret_cast<DirectX::XMFLOAT2*>(&mesh.mTextureCoords[0][i])
			);
		}
	}
	else if (!hasDiffuseMap && !hasNormalMap && !hasSpecMap)
	{
		for (unsigned int i = 0; i < mesh.mNumVertices; i++)
		{
			vbuf.EmplaceBack(
				DirectX::XMFLOAT3(mesh.mVertices[i].x * 1.0f, mesh.mVertices[i].y * 1.0f, mesh.mVertices[i].z * 1.0f),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mNormals[i])
			);
		}
	}
	return vbuf;
}


std::vector<unsigned short> Material::ExtractIndices(const aiMesh& mesh) const noexcept
{
	std::vector<unsigned short> indices;
	indices.reserve(mesh.mNumFaces * 3);
	for (unsigned int i = 0; i < mesh.mNumFaces; i++)
	{
		const auto& face = mesh.mFaces[i];
		assert(face.mNumIndices == 3);
		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
	}
	return indices;
}
std::unique_ptr<VertexBuffer> Material::MakeVertexBindable(Graphics& gfx, const aiMesh& mesh) const
{
	return std::make_unique<VertexBuffer>(gfx, ExtractVertices(mesh));
}
std::unique_ptr<IndexBuffer> Material::MakeIndexBindable(Graphics& gfx, const aiMesh& mesh) const
{
	return std::make_unique<IndexBuffer>(gfx, ExtractIndices(mesh));
}
std::vector<Technique> Material::GetTechniques() const noexcept
{
	return techniques;
}
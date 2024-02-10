#pragma once
#include "Graphics.h"
#include "BindableBase.h"
#include <vector>
#include <filesystem>
#include "Technique.h"

struct aiMaterial;
struct aiMesh;


class Material
{
public:
	Material(Graphics& gfx, const aiMaterial& material, const std::filesystem::path& path);
	CubeR::VertexBuffer ExtractVertices(const aiMesh& mesh) const noexcept;
	std::vector<unsigned short> ExtractIndices(const aiMesh& mesh) const noexcept;
	std::unique_ptr<VertexBuffer> MakeVertexBindable(Graphics& gfx, const aiMesh& mesh) const;
	std::unique_ptr<IndexBuffer> MakeIndexBindable(Graphics& gfx, const aiMesh& mesh) const;
	std::vector<Technique> GetTechniques() const noexcept;
private:
	CubeR::VertexLayout vtxLayout;
	std::vector<Technique> techniques;
	std::string modelPath;
	std::string name;
	bool hasSpecMap = false;
	bool hasNormalMap = false;
	bool hasDiffuseMap = false;
	bool hasAlphaGloss = false;
};
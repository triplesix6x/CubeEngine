#pragma once
#include "DrawableBase.h"
#include "BindableBase.h"
#include "Vertex.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <optional>
#include <filesystem>
#include <memory>
#include <assimp/postprocess.h>
#include <type_traits>
#include "../imgui/imgui.h"

class Mesh : public DrawableBase<Mesh>
{
public:
	Mesh(Graphics& gfx, std::vector<std::unique_ptr<Bindable>> bindPtrs);
	void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const ;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	mutable DirectX::XMFLOAT4X4 transform;
};

class Node
{
	friend class Model;
	friend class ModelWindow;
public:
	struct PSMaterialConstantFullmonte
	{
		BOOL  normalMapEnabled = TRUE;
		BOOL specularMapEnabled = TRUE;
		BOOL  hasGlossMap = FALSE;
		float specularPower = 3.1f;
		DirectX::XMFLOAT3 specularColor = { 0.75f,0.75f,0.75f };
		float specularMapWeight = 0.671f;
	};
	struct PSMaterialConstantNotex
	{
		DirectX::XMFLOAT3 color;
		float specularIntensity = 0.65f;
		float specularPower = 2.0f;
		float padding[3];
	};
	Node(int id, const std::string& name,std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform) ;
	Node(const Node&) = delete;
	Node& operator=(const Node&) = delete;
	void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const;
	void SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept;
	const DirectX::XMFLOAT4X4& GetAppliedTransform() const noexcept;
	int GetId() const noexcept;
	std::string GetName() const noexcept;
	const std::vector<std::unique_ptr<Node>>& getchildPtrs() const noexcept;
	void RenderTree(Node*& pSelectedNode) const noexcept;
private:
	void AddChild(std::unique_ptr<Node> pChild) ;
private:
	std::string name;
	int id;
	std::vector<std::unique_ptr<Node>> childPtrs;
	std::vector<Mesh*> meshPtrs;
	DirectX::XMFLOAT4X4 baseTransform;
	DirectX::XMFLOAT4X4 appliedTransform;
};

class Model
{
public:
	Model(Graphics& gfx, const std::string& fileName, int id=0, const char* modelName = "Unnamed Object");
	void Draw(Graphics& gfx) const;
	void ShowWindow(Graphics& gfx, Model* pSelectedModel) noexcept;
	void SetRootTransfotm(DirectX::FXMMATRIX tf);
	~Model() noexcept;
	int GetId() const noexcept;
	Node& getpRoot();
	std::string modelName;
private:
	DirectX::XMMATRIX GetTransform() const noexcept;
	Node* GetSelectedNode() const noexcept;
	static std::unique_ptr<Mesh> ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial *const *pMaterials,const std::filesystem::path& filePath);
	std::unique_ptr<Node> ParseNode(int& nextId, const aiNode& node);
	std::unique_ptr<Node> pRoot;
	std::vector<std::unique_ptr<Mesh>> meshPtrs;
	std::string rootPath;
	Node* pSelectedNode;
	int id;
	struct TransformParameters
	{
		float roll = 0.0f;
		float pitch = 0.0f;
		float yaw = 0.0f;
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
	};
	std::unordered_map<int, TransformParameters> poses;
	struct ScaleParameters
	{
		float xscale = 1.0f;
		float yscale = 1.0f;
		float zscale = 1.0f;
	};
	std::unordered_map<int, ScaleParameters> scales;
};
#pragma once
#include "DrawableBase.h"
#include "BindableBase.h"
#include "Vertex.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <optional>
#include <assimp/postprocess.h>

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
	Node(int id, const std::string& name,std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform) ;
	void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const;
	void SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept;
	int GetId() const noexcept;
	std::string GetName() const noexcept;
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
	Model(Graphics& gfx, const std::string fileName, int id=0, bool istextured=false, const char* modelName = "Unnamed Object");
	void Draw(Graphics& gfx) const;
	void ShowWindow(Model* pSelectedModel) noexcept;
	~Model() noexcept;
	int GetId() const noexcept;
	std::string modelName;
private:
	DirectX::XMMATRIX GetTransform() const noexcept;
	Node* GetSelectedNode() const noexcept;
	static std::unique_ptr<Mesh> ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial *const *pMaterials, bool istextured, std::string fileName, std::string modelName);
	std::unique_ptr<Node> ParseNode(int& nextId, const aiNode& node);
	std::unique_ptr<Node> pRoot;
	std::vector<std::unique_ptr<Mesh>> meshPtrs;
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
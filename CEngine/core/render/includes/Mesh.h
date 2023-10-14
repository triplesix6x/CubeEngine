#pragma once
#include "DrawableBase.h"
#include "BindableBase.h"
#include "Vertex.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <optional>
#include <filesystem>
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
		float specularPower = 3.1f;
		DirectX::XMFLOAT3 specularColor = { 0.75f,0.75f,0.75f };
		float specularMapWeight = 0.671f;
		float padding;
	};
	struct PSMaterialConstantNotex
	{
		DirectX::XMFLOAT3 color = { 0.447970f,0.327254f,0.176283f};
		float specularIntensity = 0.65f;
		float specularPower = 2.0f;
		float padding[3];
	};
	Node(int id, const std::string& name,std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform) ;
	void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const;
	void SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept;
	int GetId() const noexcept;
	std::string GetName() const noexcept;
	void RenderTree(Node*& pSelectedNode) const noexcept;
	template<class T>
	bool specWndContorl(Graphics& gfx, T& c)
	{
		if (meshPtrs.empty())
		{
			return false;
		}

		if constexpr (std::is_same<T, PSMaterialConstantFullmonte>::value)
		{
			if (auto pcb = meshPtrs.front()->QueryBindable<PixelConstantBuffer<T>>())
			{
				ImGui::SeparatorText("MATERIAL");

				bool normalMapEnabled = (bool)c.normalMapEnabled;
				ImGui::Checkbox("Norm Map", &normalMapEnabled);
				c.normalMapEnabled = normalMapEnabled ? TRUE : FALSE;

				bool specularMapEnabled = (bool)c.specularMapEnabled;
				ImGui::Checkbox("Spec Map", &specularMapEnabled);
				c.specularMapEnabled = specularMapEnabled ? TRUE : FALSE;

				ImGui::SliderFloat("Spec Weight", &c.specularMapWeight, 0.0f, 2.0f);

				ImGui::SliderFloat("Spec Pow", &c.specularPower, 0.0f, 1000.0f, "%f");

				ImGui::ColorPicker3("Spec Color", reinterpret_cast<float*>(&c.specularColor));
				if (ImGui::Button("Reset Material"))
				{
					c.specularPower = 3.1f;
					c.normalMapEnabled = TRUE;
					c.specularMapEnabled = TRUE;
					c.specularColor = { 0.75f,0.75f,0.75f };
					c.specularMapWeight = 0.671f;
				}
				pcb->Update(gfx, c);
				return true;
			}
		}
		else if constexpr (std::is_same<T, PSMaterialConstantNotex>::value)
		{
			if (auto pcb = meshPtrs.front()->QueryBindable<PixelConstantBuffer<T>>())
			{
				ImGui::SeparatorText("MATERIAL");

				ImGui::SliderFloat("Spec Inten.", &c.specularIntensity, 0.0f, 1.0f);

				ImGui::SliderFloat("Spec Pow", &c.specularPower, 0.0f, 1000.0f, "%f");

				ImGui::ColorPicker3("Diff Color", reinterpret_cast<float*>(&c.color));

				if (ImGui::Button("Reset Material"))
				{
					c.color = { 0.447970f,0.327254f,0.176283f };
					c.specularIntensity = 0.65f;
					c.specularPower = 2.0f;
				}

				pcb->Update(gfx, c);
				return true;
			}
		}
		return false;
	}

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
	std::string modelName;
private:
	DirectX::XMMATRIX GetTransform() const noexcept;
	Node* GetSelectedNode() const noexcept;
	static std::unique_ptr<Mesh> ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial *const *pMaterials,const std::filesystem::path& filePath);
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
	Node::PSMaterialConstantFullmonte mc;
	Node::PSMaterialConstantNotex ntx;
	std::unordered_map<int, TransformParameters> poses;
	struct ScaleParameters
	{
		float xscale = 1.0f;
		float yscale = 1.0f;
		float zscale = 1.0f;
	};
	std::unordered_map<int, ScaleParameters> scales;
};
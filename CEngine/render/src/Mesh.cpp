#include "../includes/Mesh.h"
#include "imgui.h"
#include "../core/includes/CXM.h"
#include <unordered_map>
#include <memory>
#include <libloaderapi.h>
#include "../includes/Material.h"


std::string operator-(std::string source, const std::string& target)
{
	for(size_t pos, size{target.size() }; (pos = source.find(target)) != std::string::npos;)
		source.erase(source.cbegin() + pos,source.cbegin() + (pos + size));
	return source;
}


Mesh::Mesh(Graphics& gfx, const Material& mat, const aiMesh& mesh) noexcept
	:
Drawable(gfx, mat, mesh)
{}


void Mesh::Submit(FrameCommander& frame, DirectX::FXMMATRIX accumulatedTranform) const noexcept
{
	DirectX::XMStoreFloat4x4(&transform, accumulatedTranform);
	Drawable::Submit(frame);
}


DirectX::XMMATRIX Mesh::GetTransformXM() const noexcept
{
	return DirectX::XMLoadFloat4x4(&transform);
}



Node::Node(int id, const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform)
	:
id(id), meshPtrs(std::move(meshPtrs)) , name(name)
{
	DirectX::XMStoreFloat4x4(&baseTransform, transform);
	DirectX::XMStoreFloat4x4(&appliedTransform, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&appliedScale, DirectX::XMMatrixIdentity());
}


void Node::Submit(FrameCommander& frame, DirectX::FXMMATRIX accumulatedTransform) const noexcept
{
	const auto built =
		DirectX::XMLoadFloat4x4(&appliedTransform) *
		DirectX::XMLoadFloat4x4(&baseTransform) *
		accumulatedTransform;
	for (const auto pm : meshPtrs)
	{
		pm->Submit(frame, built);
	}
	for (const auto& pc : childPtrs)
	{
		pc->Submit(frame, built);
	}
}


void Node::AddChild(std::unique_ptr<Node> pChild) 
{
	assert(pChild);
	childPtrs.push_back(std::move(pChild));
}


void Node::RenderTree(Node*& pSelectedNode) const noexcept
{
	const int selectedId = (pSelectedNode == nullptr) ? -1 : pSelectedNode->GetId();
	const auto node_flags = ImGuiTreeNodeFlags_NavLeftJumpsBackHere | ImGuiTreeNodeFlags_OpenOnArrow
		| ((GetId() == selectedId) ? ImGuiTreeNodeFlags_Selected : 0)
		| ((childPtrs.size() == 0) ? ImGuiTreeNodeFlags_Leaf : 0);
	const auto expanded = ImGui::TreeNodeEx((void*)(intptr_t)GetId(), node_flags, name.c_str());
	
		if (ImGui::IsItemClicked() || ImGui::IsItemActivated() || ImGui::IsItemFocused())
		{
			pSelectedNode = const_cast<Node*>(this);
		}
		if (expanded)
		{
			for (const auto& pChild : childPtrs)
			{
				pChild->RenderTree(pSelectedNode);
			}
			ImGui::TreePop();
		}
}

const std::vector<std::unique_ptr<Node>>& Node::getchildPtrs() const noexcept
{
	return childPtrs;
}



void Node::SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept
{
	DirectX::XMStoreFloat4x4(&appliedTransform, transform);
}

void Node::SetAppliedScale(DirectX::FXMMATRIX scale) noexcept
{
	DirectX::XMStoreFloat4x4(&appliedScale, scale);
}



int Node::GetId() const noexcept
{
	return id;
}



std::string Node::GetName() const noexcept
{
	return name;
}



Model::Model(Graphics& gfx, const std::string& fileName, int id, std::string modelName) :
	modelName(modelName), id(id), rootPath(fileName)
{
	Assimp::Importer imp;
	const auto pScene = imp.ReadFile(fileName.c_str(),
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ConvertToLeftHanded |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace);
	if (pScene == NULL)
	{
		MessageBoxA(nullptr, imp.GetErrorString(), "Standart Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	else {
		std::vector<Material> materials;
		materials.reserve(pScene->mNumMaterials);
		for (size_t i = 0; i < pScene->mNumMaterials; ++i)
		{
			materials.emplace_back(gfx, *pScene->mMaterials[i], fileName);
		}


		for (size_t i = 0; i < pScene->mNumMeshes; i++)
		{
			const auto& mesh = *pScene->mMeshes[i];
			meshPtrs.emplace_back(std::make_unique<Mesh>(gfx, materials[mesh.mMaterialIndex], mesh));
		}
		int nextId = 0;
		pRoot = ParseNode(nextId, *pScene->mRootNode);
		CUBE_TRACE(std::string("Successfully loaded model") + fileName);
	}
}

Node& Model::getpRoot()
{
	return *pRoot;
}


void Model::Submit(FrameCommander& frame) const noexcept
{
	if (auto node = GetSelectedNode())
	{
		node->SetAppliedTransform(GetTransform());
		node->SetAppliedScale(GetScale());
	}
	pRoot->Submit(frame, DirectX::XMMatrixIdentity());
}


void Model::ShowWindow(Graphics& gfx, Model* pSelectedModel) noexcept
{
	int nodeIndexTracker = 0;

	if (pSelectedModel == this)
	{
		pRoot->RenderTree(pSelectedNode);
		if (pSelectedNode != nullptr)
		{
			ImGui::SetNextWindowSize({ 400, 340 }, ImGuiCond_FirstUseEver);
			ImGui::Begin("Proprieties");


			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), modelName.c_str());
			if (ImGui::InputText("Model Name", buffer, sizeof(buffer)))
			{
				modelName = std::string(buffer);
			}

			const auto id = pSelectedNode->GetId();
			auto i = poses.find(id);
			auto j = scales.find(id);
			if (i == poses.end())
			{
				const auto& applied = pSelectedNode->GetAppliedTransform();
				const auto& appliedScale = pSelectedNode->GetAppliedScale();
				const auto angles = ExtractEulerAngles(applied);
				const auto translation = ExtractTranslation(applied);
				TransformParameters tp;
				tp.roll = angles.x;
				tp.pitch = angles.y;
				tp.yaw = angles.z;
				tp.x = translation.x;
				tp.y = translation.y;
				tp.z = translation.z;
				std::tie(i, std::ignore) = poses.insert({ id,tp });
			}
			auto& pos = i->second;

			if (j == scales.end())
			{
				const auto& appliedScale = pSelectedNode->GetAppliedScale();
				const auto nodescales = ExtractScaling(appliedScale);
				ScaleParameters sc; 
				sc.xscale = nodescales.x; 
				sc.yscale = nodescales.y; 
				sc.zscale = nodescales.z; 
				std::tie(j, std::ignore) = scales.insert({ id, sc }); 
			}
			auto& scale = j->second;

			ImGui::Columns(2);
			ImGui::SetColumnWidth(0, 100);
			ImGui::Text("Position");
			ImGui::PushID("Position");
			ImGui::NextColumn();
			ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.3f, 0.2f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });

			if (ImGui::Button("X", buttonSize))
				pos.x = 0.0f;
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			ImGui::DragFloat("##X", &pos.x, 0.1f);
			ImGui::PopItemWidth();
			ImGui::SameLine();

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });

			if (ImGui::Button("Y", buttonSize))
				pos.y = 0.0f;
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			ImGui::DragFloat("##Y", &pos.y, 0.1f);
			ImGui::PopItemWidth();
			ImGui::SameLine();

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });

			if (ImGui::Button("Z", buttonSize))
				pos.z = 0.0f;
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			ImGui::DragFloat("##Z", &pos.z, 0.1f);
			ImGui::PopItemWidth();
			ImGui::Columns(1);
			ImGui::PopStyleVar();
			ImGui::PopID();

			ImGui::Columns(2);
			ImGui::SetColumnWidth(0, 100);
			ImGui::Text("Scale");
			ImGui::PushID("Scale");
			ImGui::NextColumn();
			ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.3f, 0.2f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });

			if (ImGui::Button("X", buttonSize))
				scale.xscale = 1.0f;
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			ImGui::DragFloat("##X", &scale.xscale, 0.1f, 0.1f);
			ImGui::PopItemWidth();
			ImGui::SameLine();

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });

			if (ImGui::Button("Y", buttonSize))
				scale.yscale = 1.0f;
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			ImGui::DragFloat("##Y", &scale.yscale, 0.1f, 0.1f);
			ImGui::PopItemWidth();
			ImGui::SameLine();

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });

			if (ImGui::Button("Z", buttonSize))
				scale.zscale = 1.0f;
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			ImGui::DragFloat("##Z", &scale.zscale, 0.1f, 0.1f);
			ImGui::PopItemWidth();
			ImGui::Columns(1);
			ImGui::PopStyleVar();
			ImGui::PopID();

			ImGui::Columns(2);
			ImGui::SetColumnWidth(0, 100);
			ImGui::Text("Orientation");
			ImGui::PushID("Orientation");
			ImGui::NextColumn();
			ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.3f, 0.2f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });

			if (ImGui::Button("X", buttonSize))
				pos.roll = 0.0f;
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			ImGui::SliderAngle("##X", &pos.roll, -180.0f, 180.0f);
			ImGui::PopItemWidth();
			ImGui::SameLine();

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });

			if (ImGui::Button("Y", buttonSize))
				pos.pitch = 0.0f;
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			ImGui::SliderAngle("##Y", &pos.pitch, -180.0f, 180.0f);
			ImGui::PopItemWidth();
			ImGui::SameLine();

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });

			if (ImGui::Button("Z", buttonSize))
				pos.yaw = 0.0f;
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			ImGui::SliderAngle("##Z", &pos.yaw, -180.0f, 180.0f);
			ImGui::PopItemWidth();
			ImGui::Columns(1);
			ImGui::PopStyleVar();
			ImGui::PopID();

			if (ImGui::Button("Reset"))
			{
				pos.roll = 0.0f;
				pos.pitch = 0.0f;
				pos.yaw = 0.0f;

				pos.x = 0.0f;
				pos.y = 0.0f;
				pos.z = 0.0f;

				scale.xscale = 1.0f;
				scale.yscale = 1.0f;
				scale.zscale = 1.0f;
			}

			ImGui::End();
		}
	}
}

void Model::SetRootTransfotm(DirectX::FXMMATRIX tf)
{
	pRoot->SetAppliedTransform(tf);
}

void Model::SetRootScaling(DirectX::FXMMATRIX sf)
{
	pRoot->SetAppliedScale(sf);
}

const DirectX::XMFLOAT4X4& Node::GetAppliedTransform() const noexcept
{
	return appliedTransform;
}

const DirectX::XMFLOAT4X4& Node::GetAppliedScale() const noexcept
{
	return appliedScale;
}


DirectX::XMMATRIX Model::GetTransform() const noexcept
{
	assert(pSelectedNode != nullptr);
	const auto& transform = poses.at(pSelectedNode->GetId());
	const auto& scale = scales.at(pSelectedNode->GetId());
	return
		DirectX::XMMatrixRotationRollPitchYaw(transform.roll, transform.pitch, transform.yaw) *
		DirectX::XMMatrixScaling(scale.xscale, scale.yscale, scale.zscale) *
		DirectX::XMMatrixTranslation(transform.x, transform.y, transform.z);
}

DirectX::XMMATRIX Model::GetScale() const noexcept
{
	assert(pSelectedNode != nullptr);
	const auto& scale = scales.at(pSelectedNode->GetId());
	return
		DirectX::XMMatrixScaling(scale.xscale, scale.yscale, scale.zscale);
}



Node* Model::GetSelectedNode() const noexcept
{
	return pSelectedNode;
}


Model::~Model() noexcept
{}

int Model::GetId() const noexcept
{
	return id;
}



std::unique_ptr<Node> Model::ParseNode(int& nextId, const aiNode& node)
{
	namespace dx = DirectX;
	const auto transform = dx::XMMatrixTranspose(dx::XMLoadFloat4x4(
		reinterpret_cast<const dx::XMFLOAT4X4*>(&node.mTransformation)
	));

	std::vector<Mesh*> curMeshPtrs;
	curMeshPtrs.reserve(node.mNumMeshes);
	for (size_t i = 0; i < node.mNumMeshes; i++)
	{
		const auto meshIdx = node.mMeshes[i];
		curMeshPtrs.push_back(meshPtrs.at(meshIdx).get());
	}

	auto pNode = std::make_unique<Node>(nextId++, node.mName.C_Str(), std::move(curMeshPtrs), transform);
	for (size_t i = 0; i < node.mNumChildren; i++)
	{
		pNode->AddChild(ParseNode(nextId, *node.mChildren[i]));
	}
	return pNode;
}
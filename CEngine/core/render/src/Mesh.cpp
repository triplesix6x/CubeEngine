#include "../includes/Mesh.h"
#include "../../imgui/imgui.h"
#include <unordered_map>
#include <libloaderapi.h>

std::string operator-(std::string source, const std::string& target)
{
	for
		(
			size_t pos, size{ target.size() };
			(pos = source.find(target)) != std::string::npos;
			)
		source.erase
		(
			source.cbegin() + pos,
			source.cbegin() + (pos + size)
		);

	return source;
}



Mesh::Mesh(Graphics& gfx, std::vector<std::unique_ptr<Bindable>> bindPtrs)
{
	if (!IsStaticInitialized())
	{
		AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
		AddStaticBind(std::make_unique<Rasterizer>(gfx, false));
		AddStaticBind(std::make_unique<DepthStencil>(gfx, false));
	}

	for (auto& pb : bindPtrs)
	{
		if (auto pi = dynamic_cast<IndexBuffer*>(pb.get()))
		{
			AddIndexBuffer(std::unique_ptr<IndexBuffer>{ pi });
			pb.release();
		}
		else
		{
			AddBind(std::move(pb));
		}
	}

	AddBind(std::make_unique<TransformCbuf>(gfx, *this));
}


void Mesh::Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const
{
	DirectX::XMStoreFloat4x4(&transform, accumulatedTransform);
	Drawable::Draw(gfx);
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
}


void Node::Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const 
{
	const auto built =
		DirectX::XMLoadFloat4x4(&appliedTransform) *
		DirectX::XMLoadFloat4x4(&baseTransform) *
		accumulatedTransform;
	for (const auto pm : meshPtrs)
	{
		pm->Draw(gfx, built);
	}
	for (const auto& pc : childPtrs)
	{
		pc->Draw(gfx, built);
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
	const auto node_flags = ImGuiTreeNodeFlags_OpenOnArrow
		| ((GetId() == selectedId) ? ImGuiTreeNodeFlags_Selected : 0)
		| ((childPtrs.size() == 0) ? ImGuiTreeNodeFlags_Leaf : 0);
	const auto expanded = ImGui::TreeNodeEx((void*)(intptr_t)GetId(), node_flags, name.c_str());
	
		if (ImGui::IsItemClicked())
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
		if (!expanded)
		{
			pSelectedNode = nullptr;
		}
}



void Node::SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept
{
	DirectX::XMStoreFloat4x4(&appliedTransform, transform);
}



int Node::GetId() const noexcept
{
	return id;
}



std::string Node::GetName() const noexcept
{
	return name;
}


Model::Model(Graphics& gfx, const std::string fileName, int id, bool istextured, const char* modelName) :
	modelName(modelName), id(id)
{
	Assimp::Importer imp;
	const auto pScene = imp.ReadFile(fileName.c_str(), aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
	if (pScene == NULL)
	{
		MessageBoxA(nullptr, imp.GetErrorString(), "Standart Exception", MB_OK | MB_ICONEXCLAMATION);
	}

	for (size_t i = 0; i < pScene->mNumMeshes; i++)
	{
		meshPtrs.push_back(ParseMesh(gfx, *pScene->mMeshes[i], pScene->mMaterials, istextured, fileName, pScene->mRootNode->mName.C_Str()));
	}
	int nextId = 0;
	pRoot = ParseNode(nextId, *pScene->mRootNode);
}


void Model::Draw(Graphics& gfx) const
{
	if (auto node = GetSelectedNode())
	{
		node->SetAppliedTransform(GetTransform());
	}
	pRoot->Draw(gfx, DirectX::XMMatrixIdentity());
}


void Model::ShowWindow(Model* pSelectedModel) noexcept
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

			auto& pos = poses[pSelectedNode->GetId()];
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

			auto& scale = scales[pSelectedNode->GetId()];
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


DirectX::XMMATRIX Model::GetTransform() const noexcept
{
	assert(pSelectedNode != nullptr);
	const auto& transform = poses.at(pSelectedNode->GetId());
	const auto& scale = scales.at(pSelectedNode->GetId());
	return
		DirectX::XMMatrixRotationRollPitchYaw(transform.roll, transform.pitch, transform.yaw) *
		DirectX::XMMatrixTranslation(transform.x, transform.y, transform.z) *
		DirectX::XMMatrixScaling(scale.xscale, scale.yscale, scale.zscale);
}


Node* Model::GetSelectedNode() const noexcept
{
	return pSelectedNode;
}



std::unique_ptr<Mesh> Model::ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial *const *pMaterials, bool istextured, std::string fileName, std::string modelName)
{
	namespace dx = DirectX;
	using CubeR::VertexLayout;
	if (istextured)
	{
		CubeR::VertexBuffer vbuf(std::move(
			VertexLayout{}
			.Append(VertexLayout::Position3D)
			.Append(VertexLayout::Normal)
			.Append(VertexLayout::Texture2D)
		));

		std::string dir = fileName - modelName;

		for (unsigned int i = 0; i < mesh.mNumVertices; i++)
		{
			vbuf.EmplaceBack(
				*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mVertices[i]),
				*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mNormals[i]),
				*reinterpret_cast<dx::XMFLOAT2*>(&mesh.mTextureCoords[0][i])
			);
		}
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

		std::vector<std::unique_ptr<Bindable>> bindablePtrs;

		bool hasSpecMap = false;
		float shine = 35.0f;
		if (mesh.mMaterialIndex >= 0)
		{
			using namespace std::string_literals;
			auto& material = *pMaterials[mesh.mMaterialIndex];
			aiString texFileName;
			if (material.GetTexture(aiTextureType_DIFFUSE, 0, &texFileName) == aiReturn_SUCCESS)
			{
				bindablePtrs.push_back(std::make_unique<Texture>(gfx, dir + texFileName.C_Str()));
			}
			bindablePtrs.push_back(std::make_unique<Sampler>(gfx));

			if (material.GetTexture(aiTextureType_SPECULAR, 0, &texFileName) == aiReturn_SUCCESS)
			{
				bindablePtrs.push_back(std::make_unique<Texture>(gfx, dir + texFileName.C_Str(), 1));
				hasSpecMap = true;
			}
			else
			{
				material.Get(AI_MATKEY_SHININESS, shine);
			}
		}

		bindablePtrs.push_back(std::make_unique<VertexBuffer>(gfx, vbuf));

		bindablePtrs.push_back(std::make_unique<IndexBuffer>(gfx, indices));


		auto pvs = std::make_unique<VertexShader>(gfx, L"shaders\\PhongVS.cso");


		auto pvsbc = pvs->GetBytecode();
		bindablePtrs.push_back(std::move(pvs));

		if (hasSpecMap)
		{
			bindablePtrs.push_back(std::make_unique<PixelShader>(gfx, L"shaders\\PhongSpecPS.cso"));
		}
		else
		{
			bindablePtrs.push_back(std::make_unique<PixelShader>(gfx, L"shaders\\PhongPS.cso"));

			bindablePtrs.push_back(std::make_unique<InputLayout>(gfx, vbuf.GetLayout().GetD3DLayout(), pvsbc));

			struct PSMaterialConstant
			{
				float specularIntensity = 1.6f;
				float specularPower;
				float padding[2];
			} pmc;
			pmc.specularPower = shine;
			bindablePtrs.push_back(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(gfx, pmc, 1u));
		}
		return std::make_unique<Mesh>(gfx, std::move(bindablePtrs));
	}
	else
	{
		namespace dx = DirectX;
		using CubeR::VertexLayout;

		CubeR::VertexBuffer vbuf(std::move(
			VertexLayout{}
			.Append(VertexLayout::Position3D)
			.Append(VertexLayout::Normal)
		));

		for (unsigned int i = 0; i < mesh.mNumVertices; i++)
		{
			float scale = 3.0f;
			vbuf.EmplaceBack(
				*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mVertices[i]),
				*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mNormals[i])
			);
		}

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

		std::vector<std::unique_ptr<Bindable>> bindablePtrs;

		bindablePtrs.push_back(std::make_unique<VertexBuffer>(gfx, vbuf));

		bindablePtrs.push_back(std::make_unique<IndexBuffer>(gfx, indices));

		wchar_t path[260];
		GetModuleFileName(NULL, path, 260);
		int len = wcslen(path);
		for (int i = 1; i < 15; ++i)
		{
			path[len - i] = '\0';
		}
		auto pvs = std::make_unique<VertexShader>(gfx, wcscat(path, L"shaders\\NoTexPhongVS.cso"));

		auto pvsbc = pvs->GetBytecode();
		bindablePtrs.push_back(std::move(pvs));
		wchar_t bpath[260];
		GetModuleFileName(NULL, bpath, 260);
		int blen = wcslen(bpath);
		for (int i = 1; i < 15; ++i)
		{
			bpath[blen - i] = '\0';
		}
		bindablePtrs.push_back(std::make_unique<PixelShader>(gfx, wcscat(bpath, L"shaders\\NoTexPhongPS.cso")));

		bindablePtrs.push_back(std::make_unique<InputLayout>(gfx, vbuf.GetLayout().GetD3DLayout(), pvsbc));

		struct PSMaterialConstant
		{
			DirectX::XMFLOAT3 color = { 0.7f,0.7f,0.7f };
			float specularIntensity = 0.6f;
			float specularPower = 30.0f;
			float padding[3];
		} pmc;
		bindablePtrs.push_back(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(gfx, pmc, 1u));

		return std::make_unique<Mesh>(gfx, std::move(bindablePtrs));
	}
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
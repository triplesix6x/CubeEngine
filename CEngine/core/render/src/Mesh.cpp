#include "../includes/Mesh.h"
#include "../imgui/imgui.h"
#include <unordered_map>
#include <libloaderapi.h>


Mesh::Mesh(Graphics& gfx, std::vector<std::unique_ptr<Bindable>> bindPtrs)
{
	if (!IsStaticInitialized())
	{
		AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
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



class ModelWindow
{
public:
	void Show(const char* windowName, const Node& root) noexcept
	{
		windowName = windowName ? windowName : "Model";
		int nodeIndexTracker = 0;

		if (ImGui::TreeNode(windowName))
		{
				root.RenderTree(pSelectedNode);
				if (pSelectedNode != nullptr)
				{
					std::string name = windowName + std::string(" Settings");
					ImGui::SetNextWindowSize({ 400, 340 });
					ImGui::Begin(name.c_str());
					auto& pos = poses[pSelectedNode->GetId()];
					ImGui::Text("Orientation");
					ImGui::SliderAngle("Roll", &pos.roll, -180.0f, 180.0f);
					ImGui::SliderAngle("Pitch", &pos.pitch, -180.0f, 180.0f);
					ImGui::SliderAngle("Yaw", &pos.yaw, -180.0f, 180.0f);
					ImGui::Text("Position");
					ImGui::SliderFloat("X", &pos.x, -20.0f, 20.0f);
					ImGui::SliderFloat("Y", &pos.y, -20.0f, 20.0f);
					ImGui::SliderFloat("Z", &pos.z, -20.0f, 20.0f);
					auto& scale = scales[pSelectedNode->GetId()];
					ImGui::Text("Scale");
					ImGui::SliderFloat("X Scale", &scale.xscale, 0.1f, 20.0f);
					ImGui::SliderFloat("Y Scale", &scale.yscale, 0.1f, 20.0f);
					ImGui::SliderFloat("Z Scale", &scale.zscale, 0.1f, 20.0f);
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
				ImGui::TreePop();
				ImGui::Spacing();
		}
	}
	DirectX::XMMATRIX GetTransform() const noexcept
	{
		assert(pSelectedNode != nullptr);
		const auto& transform = poses.at(pSelectedNode->GetId());
		const auto& scale = scales.at(pSelectedNode->GetId());
		return
			DirectX::XMMatrixRotationRollPitchYaw(transform.roll, transform.pitch, transform.yaw) *
			DirectX::XMMatrixTranslation(transform.x, transform.y, transform.z) *
			DirectX::XMMatrixScaling(scale.xscale,scale.yscale,scale.zscale);
	}
	Node* GetSelectedNode() const noexcept
	{
		return pSelectedNode;
	}
private:
	Node* pSelectedNode;
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



Model::Model(Graphics& gfx, const std::string fileName) :
	pWindow(std::make_unique<ModelWindow>())
{
	Assimp::Importer imp;
	const auto pScene = imp.ReadFile(fileName.c_str(), aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
	if (pScene == NULL)
	{
		MessageBoxA(nullptr, imp.GetErrorString(), "Standart Exception", MB_OK | MB_ICONEXCLAMATION);
	}

	for (size_t i = 0; i < pScene->mNumMeshes; i++)
	{
		meshPtrs.push_back(ParseMesh(gfx, *pScene->mMeshes[i]));
	}
	int nextId = 0;
	pRoot = ParseNode(nextId, *pScene->mRootNode);
}
void Model::Draw(Graphics& gfx) const
{

	if (auto node = pWindow->GetSelectedNode())
	{
		node->SetAppliedTransform(pWindow->GetTransform());
	}
	pRoot->Draw(gfx, DirectX::XMMatrixIdentity());
}
void Model::ShowWindow(const char* windowName) noexcept
{
	pWindow->Show(windowName, *pRoot);
}
std::unique_ptr<Mesh> Model::ParseMesh(Graphics& gfx, const aiMesh& mesh)
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
	auto pvs = std::make_unique<VertexShader>(gfx, wcscat(path, L"shaders\\PhongVS.cso"));

	auto pvsbc = pvs->GetBytecode();
	bindablePtrs.push_back(std::move(pvs));
	wchar_t bpath[260];
	GetModuleFileName(NULL, bpath, 260);
	int blen = wcslen(bpath);
	for (int i = 1; i < 15; ++i)
	{
		bpath[len - i] = '\0';
	}
	bindablePtrs.push_back(std::make_unique<PixelShader>(gfx, wcscat(bpath, L"shaders\\PhongPS.cso")));

	bindablePtrs.push_back(std::make_unique<InputLayout>(gfx, vbuf.GetLayout().GetD3DLayout(), pvsbc));

	struct PSMaterialConstant
	{
		DirectX::XMFLOAT3 color = { 0.6f,0.6f,0.8f };
		float specularIntensity = 0.6f;
		float specularPower = 30.0f;
		float padding[3];
	} pmc;
	bindablePtrs.push_back(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(gfx, pmc, 1u));

	return std::make_unique<Mesh>(gfx, std::move(bindablePtrs));
}

Model::~Model() noexcept
{}

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
#include "pch.h"
#include "MeshRenderUI.h"

#include "CImGuiMgr.h"
#include "ListUI.h"

#include <Engine/CResMgr.h>
#include <Engine/CMeshRender.h>
#include <Engine/CMesh.h>
#include <Engine/CMaterial.h>

#include "TextureToolUI.h"
#include "ImGui/imgui_stdlib.h"

MeshRenderUI::MeshRenderUI()
	:
	ComponentUI("MeshRender", COMPONENT_TYPE::MESHRENDER)
  , m_pDroppedData{nullptr}
  , m_pMeshRender{nullptr}
{
	SetSize(Vec2{0, 150});
}

MeshRenderUI::~MeshRenderUI() {}

void MeshRenderUI::update()
{
	CGameObject* pTargetObj = GetTargetObject();
	if (nullptr == pTargetObj) { return; }

	m_pMeshRender = pTargetObj->MeshRender();

	m_pMesh             = m_pMeshRender->GetMesh();
	m_pSelectedMaterial = m_pMeshRender->GetMaterial();
	m_materialName.clear();
	m_meshName.clear();

	ComponentUI::update();
}

void MeshRenderUI::render_update()
{
	ComponentUI::render_update();

	ImGui::PushItemWidth(200);

	DrawMeshButton();
	DrawMaterialButton();
	DrawSTDMaterialSettingInfo();
	ImGui::PopItemWidth();
}

void MeshRenderUI::DrawSTDMaterialSettingInfo()
{
	/*if ("material\\Std2DMtrl.mtrl" == m_materialName
	    || "material\\PlayerHPBar.mtrl" == m_materialName
	    || "material\\AfterImage.mtrl" == m_materialName)
	{*/
		std::string name = ToString(m_pMeshRender->GetSpriteName());
		name             = (name.empty()) ? "NONE" : name;

		ImGui::Text("Current Sprite : %s", name.c_str());

		if (ImGui::Button("Open Texture Tool"))
		{
			TextureToolUI* pTextureToolUI = static_cast<TextureToolUI*>(CImGuiMgr::GetInst()->FindUI("TextureTool"));
			assert(pTextureToolUI);
			if (pTextureToolUI->IsActive()) { return; }

			pTextureToolUI->Activate();
			pTextureToolUI->SetTarget(m_pMeshRender->GetOwner());
		}
	//}
}

void MeshRenderUI::MeshSelect(void* pMeshName)
{
	const std::string  selectedName = static_cast<char*>(pMeshName);
	const std::wstring meshKey      = ToWString(selectedName);

	Ptr<CMesh> pMesh = CResMgr::GetInst()->FindRes<CMesh>(meshKey);
	assert(pMesh.Get());

	CMeshRender* pMeshRender = GetTargetObject()->MeshRender();
	pMeshRender->SetMesh(pMesh);
}

void MeshRenderUI::MaterialSelect(void* pMaterialName)
{
	const std::string  selectedName = static_cast<char*>(pMaterialName);
	const std::wstring materialKey  = ToWString(selectedName);

	Ptr<CMaterial> pMaterial = CResMgr::GetInst()->FindRes<CMaterial>(materialKey);
	assert(pMaterial.Get());

	CMeshRender* pMeshRender = GetTargetObject()->MeshRender();
	pMeshRender->SetSharedMaterial(pMaterial);
}

void MeshRenderUI::DrawMeshButton()
{
	if (nullptr != m_pMesh)
		m_meshName = ToString(m_pMesh->GetKey());

	ImGui::Text("MESH");
	ImGui::SameLine(86.f);

	ImGui::InputText("##MESH_NAME", &m_meshName, ImGuiInputTextFlags_ReadOnly);
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ResourceUI"))
		{
			memcpy(&m_pDroppedData, payload->Data, sizeof(void*));
		}
		ImGui::EndDragDropTarget();
	}

	ImGui::SameLine();

	if (ImGui::Button("##MESH_BUTTON", Vec2(15, 15)))
	{
		// ListUI 활성화한다.
		const auto& mapRes  = CResMgr::GetInst()->GetResList(RES_TYPE::MESH);
		ListUI*     pListUI = static_cast<ListUI*>(CImGuiMgr::GetInst()->FindUI("##ListUI"));
		pListUI->Clear();
		pListUI->SetTitle("MESH LIST");

		for (const auto& [key, pResource] : mapRes)
		{
			pListUI->AddList(ToString(key));
		}

		pListUI->Activate();
		pListUI->SetDBCEvent(BIND_FN(MeshRenderUI::MeshSelect));
	}
}

void MeshRenderUI::DrawMaterialButton()
{
	
	if (nullptr != m_pSelectedMaterial)
		m_materialName = ToString(m_pSelectedMaterial->GetKey());

	ImGui::Text("MATERIAL");
	ImGui::SameLine(86.f);
	ImGui::InputText("##MATERIAL_NAME", &m_materialName, ImGuiInputTextFlags_ReadOnly);
	ImGui::SameLine();
	if (ImGui::Button("##MATERIAL_BUTTON", Vec2(15, 15)))
	{
		// ListUI 활성화한다.
		const map<wstring, CRes*>& mapRes  = CResMgr::GetInst()->GetResList(RES_TYPE::MATERIAL);
		ListUI*                    pListUI = static_cast<ListUI*>(CImGuiMgr::GetInst()->FindUI("##ListUI"));
		pListUI->Clear();
		pListUI->SetTitle("MATERIAL LIST");

		for (const auto& [key, pResource] : mapRes)
		{
			pListUI->AddList(ToString(key));
		}

		pListUI->Activate();
		pListUI->SetDBCEvent(BIND_FN(MeshRenderUI::MaterialSelect));
	}
}




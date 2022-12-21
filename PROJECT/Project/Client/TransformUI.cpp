#include "pch.h"
#include "TransformUI.h"

#include <Engine/CScene.h>
#include <Engine/CSceneMgr.h>
#include <Engine/CTransform.h>

#include <Engine/CLayer.h>
#include "ImGui/imgui_stdlib.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include <Engine/CKeyMgr.h>

#include "ImGui/imgui_internal.h"
TransformUI::TransformUI()
	:
	ComponentUI("Transform", COMPONENT_TYPE::TRANSFORM)
  , m_pTransform{nullptr}
  , m_selectedIndex{INVALID_LAYER_INDEX}
{
	SetSize(Vec2{0, 150});
}

TransformUI::~TransformUI() {}


void TransformUI::update()
{
	CGameObject* pTargetObj = GetTargetObject();
	if (nullptr == pTargetObj)
		return;

	m_pTransform = pTargetObj->Transform();

	m_vRelativePos      = m_pTransform->GetRelativePos();
	m_vRelativeScale    = m_pTransform->GetRelativeScale();
	m_vRelativeRot      = m_pTransform->GetRelativeRotation();
	m_selectedIndex     = m_pTransform->GetOwner()->GetLayerIndex();
	m_ignoreParentScale = m_pTransform->IsIgnoreParentScale();
	m_active            = pTargetObj->IsActive();
	ComponentUI::update();
}


void TransformUI::render_update()
{
	ComponentUI::render_update();

	CGameObject* pOwner = m_pTransform->GetOwner();
	//show Layer
	ImGui::SameLine();
	CScene*            pCurrentScene    = CSceneMgr::GetInst()->GetCurScene();
	const std::wstring currentLayerName = pCurrentScene->GetLayer(pOwner->GetLayerIndex())->GetName();

	ImGui::PushItemWidth(200); // Float3 위젯 간격 설정
	ImGui::SameLine();

	const ImVec2   windowLeftTop = ImGui::GetCursorPos();
	const ImGuiIO& io            = ImGui::GetIO();
	const ImVec2   mousePos      = io.MousePos - windowLeftTop;

	static bool changeName{};
	if (ImGui::Button("CHANGE LAYER NAME"))
	{
		ImGui::SetNextWindowPos(mousePos);

		ImGui::OpenPopup("CHANGE LAYER NAME");
		changeName = true;
	}

	if (ImGui::BeginPopupModal("CHANGE LAYER NAME", &changeName))
	{
		const std::string oldName    = ToString(pCurrentScene->GetLayer(m_selectedIndex)->GetName());
		static bool       isTyping   = false;
		static bool       isHitEnter = false;

		if (false == isTyping)
		{
			m_layerName = (oldName.empty()) ? "NO_NAME" : oldName;
		}

		CKeyMgr::GetInst()->SetEnableTyping(true);
		if (ImGui::InputText("##LAYER_NAME"
		                   , &m_layerName
		                   , ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_EnterReturnsTrue))
		{
			isTyping = true;
			if (KEY_TAP(KEY::ENTER))
			{
				isHitEnter = true;
			}
		}
		if (ImGui::Button("APPLY") || isHitEnter)
		{
			pCurrentScene->SetLayerName(m_selectedIndex, ToWString(m_layerName));
			CKeyMgr::GetInst()->SetEnableTyping(false);
			isTyping   = false;
			isHitEnter = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	ImGui::SameLine();
	std::string name = std::to_string(pOwner->GetLayerIndex()) + " : ";
	if (currentLayerName.empty())
		name += "NO_NAME";
	else
		name += ToString(currentLayerName);

	if (ImGui::BeginCombo("##LAYER", name.c_str()))
	{
		for (int i = 0; i < MAX_LAYER; ++i)
		{
			const CLayer* pLayer    = pCurrentScene->GetLayer(i);
			std::wstring  layerName = pLayer->GetName();
			std::string   showName  = std::to_string(i) + " : ";
			if (layerName.empty())
				showName += "NO_NAME";
			else
				showName += ToString(layerName);

			if (ImGui::Selectable(showName.c_str()))
			{
				CSceneMgr::GetInst()->ChangeObjectLayer(pOwner, i);
				m_selectedIndex = i;
			}
		}
		ImGui::EndCombo();
	}

	m_vRelativeRot.ToDegree();

	ImGui::Text("Relative Position");
	ImGui::SameLine();
	ImGui::InputFloat3("##TRANSFORM_POSITION", m_vRelativePos);
	m_pTransform->SetRelativePos(m_vRelativePos);

	ImGui::Text("Relative Scale   ");
	ImGui::SameLine();
	ImGui::InputFloat3("##TRANSFORM_SCALE", m_vRelativeScale);
	m_pTransform->SetRelativeScale(m_vRelativeScale);

	ImGui::Text("Relative Rotation");
	ImGui::SameLine();
	ImGui::InputFloat3("##TRANSFORM_ROTATION", m_vRelativeRot);
	m_vRelativeRot.ToRadian();
	m_pTransform->SetRelativeRotation(m_vRelativeRot);

	ImGui::Text("Ignore parent scale");
	ImGui::SameLine();
	ImGui::Checkbox("##IGNORE_PARENT_SCALE", &m_ignoreParentScale);
	m_pTransform->SetIgnoreParentScale(m_ignoreParentScale);

	ImGui::SameLine();
	ImGui::Text("On/Off GameObject");
	ImGui::SameLine();
	if (ImGui::Checkbox("##ENABLE_DISABLE_GAME_OBJECT", &m_active))
	{
		if (m_active)
			GetTargetObject()->Activate();
		else
			GetTargetObject()->Deactivate();
	}
	ImGui::PopItemWidth();
}

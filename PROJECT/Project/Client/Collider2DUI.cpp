#include "pch.h"
#include "Collider2DUI.h"

#include <Engine/CCollider2D.h>
#include <Engine/CCollisionMgr.h>
#include <Engine/CScene.h>
#include <Engine/CSceneMgr.h>

#include <Engine/CLayer.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "ImGui/imgui_internal.h"
#include "ImGui/imgui_stdlib.h"

#undef max
Collider2DUI::Collider2DUI()
	:
	ComponentUI{"Collider2D", COMPONENT_TYPE::COLLIDER2D}
  , m_pCollider2D{nullptr}
  , m_colliderType{}
  , m_arrCheckLayer{}
  , m_selectedIndex{0}
  , m_maxItemWidth{0}
{
	SetSize(Vec2{0, 150});
}

Collider2DUI::~Collider2DUI() {}

void Collider2DUI::update()
{
	CGameObject* pTargetObj = GetTargetObject();

	if (nullptr == pTargetObj) { return; }

	m_pCollider2D = pTargetObj->Collider2D();

	m_offsetPos    = m_pCollider2D->GetOffsetPos();
	m_offsetScale  = m_pCollider2D->GetOffsetScale();
	m_colliderType = m_pCollider2D->GetCollider2DType();

	ComponentUI::update();
}

void Collider2DUI::render_update()
{
	ComponentUI::render_update();

	ImGui::PushItemWidth(200); // Float3 위젯 간격 설정

	ImGui::Text("Position");
	ImGui::SameLine(100);
	ImGui::InputFloat2("##COLLISION_POSITION", &m_offsetPos[0]);
	m_pCollider2D->SetOffsetPos(m_offsetPos);

	ImGui::Text("Scale");
	ImGui::SameLine(100);
	ImGui::InputFloat2("##COLLISION_SCALE", &m_offsetScale[0]);
	m_pCollider2D->SetOffsetScale(m_offsetScale);

	ImGui::Text("ColliderType");
	ImGui::SameLine(100);

	static const char* type[2] = {"BOX", "CIRCLE"};
	if (ImGui::BeginCombo("##COLLIDER_TYPE_COMBO", type[static_cast<int>(m_colliderType)]))
	{
		for (int i = 0; i < std::size(type); ++i)
		{
			if (ImGui::Selectable(type[i]))
			{
				m_pCollider2D->SetCollider2DType(static_cast<COLLIDER2D_TYPE>(i));
			}
		}
		ImGui::EndCombo();
	}

	static bool open{};
	if (ImGui::Button("Check Collision Between"))
	{
		ImGui::OpenPopup("CHECK_COLLISION");
		open = true;
	}

	if (ImGui::BeginPopupModal("CHECK_COLLISION", &open))
	{
		const CScene* pCurrentScene = CSceneMgr::GetInst()->GetCurScene();

		ImGui::BeginGroup();
		//대상
		ImGui::Text("SELECT TARGET");
		for (int i = 0; i < MAX_LAYER; ++i)
		{
			const CLayer* pLayer    = pCurrentScene->GetLayer(i);
			std::string   layerName = ToString(pLayer->GetName());

			std::string name{};
			if (layerName.empty())
				name = std::to_string(pLayer->GetLayerIdx()) + " : NO_NAME";
			else
				name = std::to_string(pLayer->GetLayerIdx()) + " : " + layerName;

			ImGui::RadioButton(name.c_str(), &m_selectedIndex, i);


			m_maxItemWidth = std::max(m_maxItemWidth, ImGui::GetItemRectSize().x);
			if (0 != i % 6 || 0 == i)
			{
				if (6 <= i)
					ImGui::SameLine(m_maxItemWidth * (i % 6));
				else
					ImGui::SameLine(m_maxItemWidth * ((i + 1) % 6));
			}
		}
		ImGui::EndGroup();

		ImGui::Separator();
		//선택할 것들
		ImGui::BeginGroup();
		ImGui::Text("SELECT LAYER");
		for (int i = 0; i < std::size(m_arrCheckLayer); ++i)
		{
			const CLayer* pLayer    = pCurrentScene->GetLayer(i);
			std::string   layerName = ToString(pLayer->GetName());

			std::string name{};
			if (layerName.empty())
				name = std::to_string(pLayer->GetLayerIdx()) + " : NO_NAME";
			else
				name = std::to_string(pLayer->GetLayerIdx()) + " : " + layerName;
			//위의 target과 이름이 겹치므로 이를 방지
			name += "##SELECT";

			ImGui::Checkbox(name.c_str(), &m_arrCheckLayer[i]);

			if (0 != i % 6 || 0 == i)
			{
				if (6 <= i)
					ImGui::SameLine(m_maxItemWidth * (i % 6));
				else
					ImGui::SameLine(m_maxItemWidth * ((i + 1) % 6));
			}
		}
		ImGui::EndGroup();
		for (int i = 0; i < std::size(m_arrCheckLayer); ++i)
		{
			if (m_arrCheckLayer[i])
				CCollisionMgr::GetInst()->CollisionCheck(m_selectedIndex, i);
			else
				CCollisionMgr::GetInst()->CollisionOff(m_selectedIndex, i);
		}

		//End PopUp
		ImGui::EndPopup();
	}
}

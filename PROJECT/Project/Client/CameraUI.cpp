#include "pch.h"
#include "CameraUI.h"


#include <Engine/CCamera.h>
#include <Engine/CSceneMgr.h>
#include <Engine/CScene.h>
#include <Engine/CLayer.h>
CameraUI::CameraUI()
	:
	ComponentUI("Camera", COMPONENT_TYPE::CAMERA)
{
	SetSize(Vec2{0, 200});
}

CameraUI::~CameraUI() {}


void CameraUI::update()
{
	const CGameObject* pTarget = GetTargetObject();
	if (nullptr == pTarget)
		return;

	m_pCamera   = GetTargetObject()->Camera();
	m_layerMask = m_pCamera->GetLayerMask();
	ComponentUI::update();
}

void CameraUI::render_update()
{
	ComponentUI::render_update();

	// PROJECTION TYPE
	ImGui::Text("Projection Type");
	ImGui::SameLine();

	PROJ_TYPE type = m_pCamera->GetProjType();

	static const char* projectionType[2] = {"ORTHOGRAPHIC", "PERSPECTIVE"};

	if (ImGui::BeginCombo("##ProjType", projectionType[static_cast<int>(type)]))
	{
		for (int i = 0; i < std::size(projectionType); ++i)
		{
			if (ImGui::Selectable(projectionType[i]))
			{
				m_pCamera->SetProjType(static_cast<PROJ_TYPE>(i));
			}
		}

		ImGui::EndCombo();
	}

	// 투영 가로길이
	ImGui::Text("Width");
	ImGui::SameLine(100);
	float fWidth = m_pCamera->GetWidth();

	if (PROJ_TYPE::PERSPECTIVE == type)
	{
		ImGui::GetStyle().Alpha = 0.25f;
		ImGui::InputFloat("##InputWidth", &fWidth, 0.f, 0.f, "%.3f", ImGuiInputTextFlags_ReadOnly);
		ImGui::GetStyle().Alpha = 1.f;
	}

	else
		ImGui::InputFloat("##InputWidth", &fWidth);

	m_pCamera->SetWidth(fWidth);

	// 종횡비
	ImGui::Text("AspectRatio");
	ImGui::SameLine(100);
	float fAspectRatio = m_pCamera->GetAspectRatio();
	ImGui::InputFloat("##InputAspectRatio", &fAspectRatio);
	m_pCamera->SetAspectRatio(fAspectRatio);


	// 시야 각      
	ImGui::Text("FOV");
	ImGui::SameLine(100);
	float fFOV = (m_pCamera->GetFOV() / XM_PI) * 180.f;

	if (PROJ_TYPE::ORTHOGRAPHIC == type)
	{
		ImGui::GetStyle().Alpha = 0.25f;
		ImGui::InputFloat("##InputFOV", &fFOV, 0.f, 0.f, "%.3f", ImGuiInputTextFlags_ReadOnly);
		ImGui::GetStyle().Alpha = 1.f;
	}
	else
		ImGui::InputFloat("##InputFOV", &fFOV);

	fFOV = (fFOV / 180.f) * XM_PI;
	m_pCamera->SetFOV(fFOV);

	// 시야 거리
	ImGui::Text("Far Distance");
	ImGui::SameLine(100);
	float fFar = m_pCamera->GetFar();
	ImGui::InputFloat("##InputFar", &fFar);
	m_pCamera->SetFar(fFar);

	if (ImGui::Button("Check All Layer"))
	{
		m_pCamera->CheckLayerMaskAll();
	}

	static bool open{};
	if (ImGui::Button("SET CAMERA MASK LAYER"))
	{
		ImGui::OpenPopup("CAMERA_MASK_LAYER");
		open = true;
	}

	if (ImGui::BeginPopupModal("CAMERA_MASK_LAYER", &open))
	{
		const CScene* pCurrentScene = CSceneMgr::GetInst()->GetCurScene();

		ImGui::BeginGroup();
		//대상
		ImGui::Text("SELECT TARGET");
		for (int i = 0; i < m_layerMask.size(); ++i)
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

			bool mask = m_layerMask[i];
			if (ImGui::Checkbox(name.c_str(), &mask))
			{
				m_pCamera->CheckLayerMask(i);
			}

			if (0 != i % 6 || 0 == i)
			{
				if (6 <= i)
					ImGui::SameLine(m_maxItemWidth * (i % 6));
				else
					ImGui::SameLine(m_maxItemWidth * ((i + 1) % 6));
			}
		}
		ImGui::EndGroup();

		ImGui::EndPopup();
	}
}

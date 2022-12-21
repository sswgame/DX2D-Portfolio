#include "pch.h"
#include "Animator2DUI.h"

#include <Engine/CAnimator2D.h>
#include <Engine/CAnimation2D.h>
#include <Engine/CMeshRender.h>

#include "AnimationToolUI.h"
#include "CImGuiMgr.h"

Animator2DUI::Animator2DUI()
	:
	ComponentUI{"Animator2D", COMPONENT_TYPE::ANIMATOR2D}
  , m_pAnimator{nullptr}
  , m_isPlaying{true}
{
	SetSize(Vec2{0, 100});
}

Animator2DUI::~Animator2DUI() = default;


void Animator2DUI::update()
{
	CGameObject* pTargetObject = GetTargetObject();
	if (nullptr == pTargetObject)
		return;

	m_pAnimator = pTargetObject->Animator2D();
	assert(m_pAnimator);

	const CAnimation2D* pCurrentAnim = m_pAnimator->GetCurrentAnim();
	if (nullptr != pCurrentAnim)
	{
		m_selectedAnimationName = pCurrentAnim->GetName();
	}
	ComponentUI::update();

}

void Animator2DUI::render_update()
{
	ComponentUI::render_update();


	ImGui::PushItemWidth(200); // Float3 위젯 간격 설정

	const std::map<std::wstring, CAnimation2D*>& mapAnimList = m_pAnimator->GetAnimationList();
	ImGui::Text("Total Animation Count : %d", mapAnimList.size());

	const CAnimation2D* pCurrentAnim = m_pAnimator->GetCurrentAnim();
	if (nullptr != pCurrentAnim)
	{
		if (ImGui::BeginCombo("##ANIMATION_LIST", ToString(pCurrentAnim->GetName()).c_str()))
		{
			for (auto& [animationName, pAnimation2D] : mapAnimList)
			{
				if (ImGui::Selectable(ToString(animationName).c_str()))
				{
					m_selectedAnimationName = animationName;
					m_pAnimator->Play(m_selectedAnimationName, true);
				}
			}
			ImGui::EndCombo();
		}
		ImGui::Text("Frame Count : %d", pCurrentAnim->GetMaxFrame());
	}
	ImGui::BeginGroup();
	if (nullptr != pCurrentAnim)
	{
		//Button Group
		if (ImGui::Button("Play##Animation")) { PlayAnimation(); }
		ImGui::SameLine();
		if (ImGui::Button("Stop##Animation")) { StopAnimation(); }
		ImGui::SameLine();
	}
	if (ImGui::Button("Edit##Animation")) { EditAnimation(); }
	ImGui::EndGroup();
}

void Animator2DUI::PlayAnimation()
{
	if (nullptr == m_pAnimator->GetCurrentAnim()) return;

	if (false == m_isPlaying)
	{
		m_isPlaying = true;
		m_pAnimator->Play(m_selectedAnimationName, true);
		m_pAnimator->GetCurrentAnim()->Reset();
	}
}

void Animator2DUI::StopAnimation()
{
	if (nullptr == m_pAnimator->GetCurrentAnim()) return;

	if (true == m_isPlaying)
	{
		m_isPlaying = false;
		m_pAnimator->Stop();
	}
}

void Animator2DUI::EditAnimation()
{
	AnimationToolUI* pAnimationToolUI = static_cast<AnimationToolUI*>(CImGuiMgr::GetInst()->FindUI("AnimationTool"));
	assert(pAnimationToolUI);
	if (pAnimationToolUI->IsActive()) return;

	pAnimationToolUI->Activate();
	pAnimationToolUI->SetTarget(m_pAnimator->GetOwner());
}

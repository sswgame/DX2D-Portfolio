#include "pch.h"
#include "CAfterImageScript.h"

#include <Engine/CAnimator2D.h>

#include <Engine/CAnimation2D.h>
#include "CObjectManagerScript.h"
#include "CPlayerScript.h"
#include "CUnitScript.h"

CAfterImageScript::CAfterImageScript()
	:
	CScript{static_cast<int>(SCRIPT_TYPE::AFTERIMAGESCRIPT), DEFINE_NAME(CAfterImageScript)}
  , m_pUnitScript{nullptr}
  , m_holdingTime{0.f}
  , m_alpha{1.f}
  , m_order{0}
  , m_showImage{false}
  , m_direction{}
  , m_pPlayerScript{nullptr}
  , m_pTargetUnitScript{nullptr} {}

CAfterImageScript::~CAfterImageScript() = default;

void CAfterImageScript::start()
{
	if (nullptr == m_pUnitScript)
	{
		CGameObject* pPlayer = CMgrScript<CObjectManagerScript>::GetInst()->GetPlayer();
		m_pTargetUnitScript = static_cast<CUnitScript*>(pPlayer->GetScript(static_cast<UINT>(SCRIPT_TYPE::UNITSCRIPT)));
		m_pPlayerScript = static_cast<CPlayerScript*>(pPlayer->GetScript(static_cast<UINT>(SCRIPT_TYPE::PLAYERSCRIPT)));

		m_pUnitScript = m_pTargetUnitScript->Clone();
		m_pUnitScript->RegisterGravityScript(nullptr);
		m_pUnitScript->SetHP(std::numeric_limits<int>::max());
		m_pUnitScript->SetMaxHP(m_pUnitScript->GetHP());
		m_pUnitScript->ResetAnimationList();

		GetOwner()->AddComponent(m_pUnitScript);

		GetOwner()->SetLayerIndex(DEFAULT_LAYER);
	}
}

void CAfterImageScript::update()
{
	m_pUnitScript->SetUnitState(m_pTargetUnitScript->GetUnitState());
	m_pUnitScript->SetUnitDirection(m_pTargetUnitScript->GetUnitDirection());

	if (false == m_pPlayerScript->IsDashState())
	{
		m_showImage   = false;
		m_holdingTime = 0.f;
		if (GetOwner()->IsInScene())
		{
			CSceneMgr::GetInst()->RemoveFromSceneButNotDelete(GetOwner());
			CMgrScript<CObjectManagerScript>::GetInst()->ReturnToPool(GetOwner());
			GetOwner()->Deactivate();
		}
	}
	else
	{
		
			if (m_showImage == false && m_holdingTime <= 0.f)
			{
				CopyTargetData();
			}
			m_holdingTime += DT;
			if (m_holdingTime > m_order * 0.02f)
			{
				/*if (false == MeshRender()->IsActive())
				{
					MeshRender()->Activate();
				}*/
				m_showImage = true;
				UpdateData();
			}
		
	}
}

void CAfterImageScript::lateupdate()
{
	// 잔상 회전 변경 막기
	if (m_showImage)
	{
		switch (m_direction)
		{
		case UNIT_DIRECTION::RIGHT:
			{
				Transform()->SetRelativeRotation(Vec3(0.f, 0.f, 0.f));
				break;
			}
		case UNIT_DIRECTION::LEFT:
			{
				Transform()->SetRelativeRotation(Vec3(0.f, XM_PI, 0.f));
				break;
			}
		default:
			break;
		}
	}
}

void CAfterImageScript::UpdateData()
{
	MeshRender()->GetMaterial()->SetScalarParam(SCALAR_PARAM::FLOAT_0, &m_alpha);
}

void CAfterImageScript::ResetObject()
{
	m_showImage   = false;
	m_holdingTime = 0.f;
	m_alpha       = 1.f;
	GetOwner()->Activate();
}

void CAfterImageScript::CopyTargetData()
{
	assert(m_pPlayerScript);

	const Vec3& playerPosition     = m_pPlayerScript->Transform()->GetRelativePos();
	const Vec3& afterImagePosition = Transform()->GetRelativePos();
	Transform()->SetRelativePos(Vec3{playerPosition.x, playerPosition.y, afterImagePosition.z});

	const CAnimation2D* pTargetAnimation = m_pPlayerScript->Animator2D()->GetCurrentAnim();
	size_t              pos              = pTargetAnimation->GetName().find(L"_");
	const std::wstring  prefix           = pTargetAnimation->GetName().substr(0, pos);
	const std::wstring& shadowName       = prefix + L"_jump_shadow";

	CAnimation2D*       pShadowAnimation = m_pPlayerScript->Animator2D()->FindAnim(shadowName);
	const tAnim2DFrame& frameInfo        = pShadowAnimation->GetFrame(0);
	MeshRender()->SetLeftTop(frameInfo.vLT);
	MeshRender()->SetScale(frameInfo.vSlice);
	MeshRender()->SetBackgroundSize(pShadowAnimation->GetBackgroundSize());
	MeshRender()->SetTexture(pTargetAnimation->GetAtlas());
	MeshRender()->SetHasMetaInfo(true);
}
